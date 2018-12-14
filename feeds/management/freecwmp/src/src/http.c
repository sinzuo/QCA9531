/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <libfreecwmp.h>
#include <libubox/uloop.h>
#include <libubox/usock.h>

#include <curl/curl.h>

#include "b64.h"

#include "http.h"

#include "config.h"
#include "cwmp.h"
#include "freecwmp.h"

#ifdef DUMMY_MODE
static char *fc_cookies = "./ext/tmp/freecwmp_cookies";
#else
static char *fc_cookies = "/tmp/freecwmp_cookies";
#endif

static struct http_client http_c;
static struct http_server http_s;

static void
http_new_client(struct uloop_fd *ufd, unsigned events);
static void
http_del_client(struct uloop_process *uproc, int ret);

int
http_client_init(void)
{
	if (asprintf(&http_c.url, "%s://%s:%s@%s:%s%s",
		     config->acs->scheme,
		     config->acs->username,
		     config->acs->password,
		     config->acs->hostname,
		     config->acs->port,
		     config->acs->path) == -1)
		return -1;

	DDF("+++ HTTP CLIENT CONFIGURATION +++\n");
	DD("url: %s\n", http_c.url);
	if (config->acs->ssl_cert)
		DD("ssl_cert: %s\n", config->acs->ssl_cert);
	if (config->acs->ssl_cacert)
		DD("ssl_cacert: %s\n", config->acs->ssl_cacert);
	if (!config->acs->ssl_verify)
		DD("ssl_verify: SSL certificate validation disabled.\n");
	DDF("--- HTTP CLIENT CONFIGURATION ---\n");

	http_c.header_list = NULL;
	http_c.header_list = curl_slist_append(http_c.header_list, "User-Agent: freecwmp");
	if (!http_c.header_list) return -1;
	http_c.header_list = curl_slist_append(http_c.header_list, "Content-Type: text/xml;charset=utf-8");
	if (!http_c.header_list) return -1;
# ifdef ACS_FUSION
	char *expect_header = "Expect:";
	http_c.header_list = curl_slist_append(http_c.header_list, expect_header);
	if (!http_c.header_list) return -1;
# endif /* ACS_FUSION */

	lfc_log_message(NAME, L_NOTICE, "configured acs url %s", http_c.url);
	return 0;
}

void
http_client_exit(void)
{
	FREE(http_c.url);

	if (http_c.header_list) {
		curl_slist_free_all(http_c.header_list);
		http_c.header_list = NULL;
	}
	if (access(fc_cookies, W_OK) == 0)
		remove(fc_cookies);

}

static size_t
http_get_response(void *buffer, size_t size, size_t rxed, void *userdata)
{
	char *c;
	char **msg_in = (char **) userdata;

	if (asprintf(&c, "%s%.*s", *msg_in, (int) (size * rxed), (char *) buffer) == -1) {
		FREE(*msg_in);
		return -1;
	}

	free(*msg_in);
	*msg_in = c;

	//DDF("+++ RECEIVED HTTP RESPONSE (PART) +++\n");
	//DDF("%.*s", (int) (size * rxed), (char *) buffer);
	//DDF("--- RECEIVED HTTP RESPONSE (PART) ---\n");

	return size * rxed;
}

int8_t
http_send_message(char *msg_out, char **msg_in)
{
	CURLcode res;
	CURL *curl;

	curl = curl_easy_init();
	if (!curl) return -1;

	curl_easy_setopt(curl, CURLOPT_URL, http_c.url);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_c.header_list);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, msg_out);
	if (msg_out)
	{
		DDF("+++ SEND HTTP REQUEST +++\n");
		DDF("%s", msg_out);
		DDF("--- SEND HTTP REQUEST ---\n");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(msg_out));
	}
	else
	{
		DDF("+++ SEND EMPTY HTTP REQUEST +++\n");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);
	}

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_get_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, msg_in);

	curl_easy_setopt( curl, CURLOPT_TIMEOUT, 10 );

# ifdef DEVEL
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
# endif

	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, fc_cookies);
	curl_easy_setopt(curl, CURLOPT_COOKIEJAR, fc_cookies);

	/* TODO: test this with real ACS configuration */
	if (config->acs->ssl_cert)
		curl_easy_setopt(curl, CURLOPT_SSLCERT, config->acs->ssl_cert);
	if (config->acs->ssl_cacert)
		curl_easy_setopt(curl, CURLOPT_CAINFO, config->acs->ssl_cacert);
	if (!config->acs->ssl_verify)
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

	if (config->acs->auth_digest)
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);

	*msg_in = (char *) calloc (1, sizeof(char));

# ifdef DEVEL
        FILE *fplog = fopen(LOG_FILE,"a+"); 
        if(fplog)
        { 
            curl_easy_setopt(curl, CURLOPT_STDERR,fplog);
        } 
# endif
	res = curl_easy_perform(curl);
# ifdef DEVEL
        fclose(fplog);
# endif
	if (!strlen(*msg_in)) {
		FREE(*msg_in);
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	if (res) return -1;

	if (*msg_in) {
		DDF("+++ RECEIVED HTTP RESPONSE +++\n");
		DDF("%s", *msg_in);
		DDF("--- RECEIVED HTTP RESPONSE ---\n");
 	} else {
		DDF("+++ RECEIVED EMPTY HTTP RESPONSE +++\n");
	}

	return 0;
}

void
http_server_init(void)
{
	http_s.http_event.cb = http_new_client;

	http_s.http_event.fd = usock(USOCK_TCP | USOCK_SERVER, config->local->ip, config->local->port);
	uloop_fd_add(&http_s.http_event, ULOOP_READ | ULOOP_EDGE_TRIGGER);

	DDF("+++ HTTP SERVER CONFIGURATION +++\n");
	if (config->local->ip)
		DDF("ip: '%s'\n", config->local->ip);
	else
		DDF("NOT BOUND TO IP\n");
	DDF("port: '%s'\n", config->local->port);
	DDF("--- HTTP SERVER CONFIGURATION ---\n");

	lfc_log_message(NAME, L_NOTICE, "http server initialized");
}

static void
http_new_client(struct uloop_fd *ufd, unsigned events)
{
	int status;
	struct timeval t;

	t.tv_sec = 60;
	t.tv_usec = 0;

	for (;;) {
		int client = accept(ufd->fd, NULL, NULL);

		/* set one minute timeout */
		if (setsockopt(ufd->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof t)) {
			DD("setsockopt() failed\n");
		}

		if (client == -1)
			break;

		struct uloop_process *uproc = calloc(1, sizeof(*uproc));
		if (!uproc || (uproc->pid = fork()) == -1) {
			free(uproc);
			close(client);
		}

		if (uproc->pid != 0) {
			/* parent */
			/* register an event handler for when the child terminates */
			uproc->cb = http_del_client;
			uloop_process_add(uproc);
			close(client);
		} else {
			/* child */
			FILE *fp;
			char buffer[BUFSIZ];
			int8_t auth_status = 0;

			fp = fdopen(client, "r+");

			DDF("+++ RECEIVED HTTP REQUEST +++\n");
			while (fgets(buffer, sizeof(buffer), fp)) {
#ifdef DEVEL
				fwrite(buffer, 1, strlen(buffer), stderr);
#endif

				if (!strncasecmp(buffer, "Authorization: Basic ", strlen("Authorization: Basic "))) {
					const char *c1, *c2, *min, *val;
					char *username = NULL;
					char *password = NULL;
					char *acs_auth_basic = NULL;
					char *auth_basic_check = NULL;
					int len;

					username = config->local->username;
					password = config->local->password;

					if (!username || !password) {
						// if we dont have username or password configured proceed with connecting to ACS
						FREE(username);
						FREE(password);
						auth_status = 1;
						goto http_end_child;
					}

					c1 = strrchr(buffer, '\r');
					c2 = strrchr(buffer, '\n');

					if (!c1)
						c1 = c2;
					if (!c2)
						c2 = c1;
					if (!c1 || !c2)
						continue;

					min = (c1 < c2) ? c1 : c2;

					val = strrchr(buffer, ' ');
					if (!val)
						continue;

					val += sizeof(char);
					size_t size = min - val;

					acs_auth_basic = (char *) zstream_b64decode(val, &size);
					if (!acs_auth_basic)
						continue;

					if (asprintf(&auth_basic_check, "%s:%s", username, password) == -1) {
						FREE(username);
						FREE(password);
						free(acs_auth_basic);
						goto error_child;
					}

					if (size == strlen(auth_basic_check)) {
						len = size;
					} else {
						auth_status = 0;
						goto free_resources;
					}

					if (!memcmp(acs_auth_basic, auth_basic_check, len * sizeof(char)))
						auth_status = 1;
					else
						auth_status = 0;

free_resources:
					free(acs_auth_basic);
					free(auth_basic_check);
				}

				if (buffer[0] == '\r' || buffer[0] == '\n') {
					/* end of http request (empty line) */
					goto http_end_child;
				}

			}
error_child:
			/* here we are because of an error, e.g. timeout */
			status = ETIMEDOUT|ENOMEM;
			goto done_child;

http_end_child:
			fflush(fp);
			if (auth_status) {
				status = 0;
				fputs("HTTP/1.1 204 No Content\r\n\r\n", fp);
			} else {
				status = EACCES;
				fputs("HTTP/1.1 401 Unauthorized\r\n", fp);
				fputs("Connection: close\r\n", fp);
				fputs("WWW-Authenticate: Basic realm=\"default\"\r\n", fp);
			}
			fputs("\r\n", fp);
			goto done_child;

done_child:
			fclose(fp);
			free(uproc);
			DDF("--- RECEIVED HTTP REQUEST ---\n");
			exit(status);
		}
	}
}

static void
http_del_client(struct uloop_process *uproc, int ret)
{
	free(uproc);

	/* child terminated ; check return code */
	if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
		DDF("+++ HTTP SERVER CONNECTION SUCCESS +++\n");
		lfc_log_message(NAME, L_NOTICE, "acs initiated connection");
		cwmp_connection_request(CONNECTION_REQUEST);
	} else {
		DDF("+++ HTTP SERVER CONNECTION FAILED +++\n");
	}
}

