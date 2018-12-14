/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#define _GNU_SOURCE

#include <errno.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <libfreecwmp.h>
#include <libubox/uloop.h>

#include "external.h"

#include "freecwmp.h"

#ifdef DUMMY_MODE
static char *fc_script = "./ext/openwrt/scripts/freecwmp.sh";
#else
static char *fc_script = "/usr/sbin/freecwmp";
#endif
static char *fc_script_set_actions = "/tmp/freecwmp_set_action_values.sh";

static struct uloop_process uproc;

int external_get_action(char *action, char *name, char **value)
{
	//lfc_log_message(NAME, L_NOTICE, "executing get %s '%s'\n",
	//		action, name);

	int pfds[2];
	char *c = NULL;

	if (pipe(pfds) < 0)
		return -1;

	if ((uproc.pid = fork()) == -1)
		goto error;

	if (uproc.pid == 0) {
		/* child */

		const char *argv[8];
		int i = 0;
		argv[i++] = "/bin/sh";
		argv[i++] = fc_script;
		argv[i++] = "--newline";
		argv[i++] = "--value";
		argv[i++] = "get";
		argv[i++] = action;
		argv[i++] = name;
		argv[i++] = NULL;

		close(pfds[0]);
		dup2(pfds[1], 1);
		close(pfds[1]);

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0)
		goto error;

	/* parent */
	close(pfds[1]);

	int status;
	while (wait(&status) != uproc.pid) {
		DD("waiting for child to exit");
	}

	char buffer[64];
	ssize_t rxed;
	int t;

	*value = NULL;
	while ((rxed = read(pfds[0], buffer, sizeof(buffer))) > 0) {
		if (*value)
			t = asprintf(&c, "%s%.*s", *value, (int) rxed, buffer);
		else
			t = asprintf(&c, "%.*s", (int) rxed, buffer);

		if (t == -1) goto error;

		free(*value);
		*value = strdup(c);
		free(c);
	}

	if (!(*value)) {
		goto done;
	}

	if (!strlen(*value)) {
		FREE(*value);
		goto done;
	}

	if (rxed < 0)
		goto error;

done:
	close(pfds[0]);
	return 0;

error:
	free(c);
	FREE(*value);
	close(pfds[0]);
	return -1;
}

int external_get_inform(char *name, char **value)
{
	//lfc_log_message(NAME, L_NOTICE, "executing get '%s',name);

	int pfds[2];
	char *c = NULL;

	if (pipe(pfds) < 0)
        {
                D("fail to create pipe:%d,%s",errno,strerror(errno)); 
		if(24 == errno)
                {
	            D("close fd");
		    int i = 10;
                    for(;i < 1024;i++)
                    {
                        close(i); 
                    }
                }
	        return -1;
        }
	if ((uproc.pid = fork()) == -1)
		goto error;
	if (uproc.pid == 0) {
		/* child */

		const char *argv[4];
		int i = 0;
		argv[i++] = "/bin/sh";
		argv[i++] = "/usr/share/freecwmp/functions/inform";
		argv[i++] = name;
		argv[i++] = NULL;

		close(pfds[0]);
		dup2(pfds[1], 1);
		close(pfds[1]);

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0)
		goto error;

	/* parent */
	close(pfds[1]);

	int status;
	while (wait(&status) != uproc.pid) {
		DD("waiting for child to exit");
	}

	char buffer[64];
	ssize_t rxed;
	int t;

	*value = NULL;
	while ((rxed = read(pfds[0], buffer, sizeof(buffer))) > 0) {
		if (*value)
			t = asprintf(&c, "%s%.*s", *value, (int) rxed, buffer);
		else
			t = asprintf(&c, "%.*s", (int) rxed, buffer);

		if (t == -1) goto error;

		free(*value);
		*value = strdup(c);
		free(c);
	}

	if (!(*value)) {
		goto done;
	}

	if (!strlen(*value)) {
		FREE(*value);
		goto done;
	}

	if (rxed < 0)
		goto error;

done:
	close(pfds[0]);
	return 0;

error:
	D("external_get_inform failed");
	free(c);
	FREE(*value);
	close(pfds[0]);
	return -1;
}


int external_set_action_write(char *action, char *name, char *value)
{
	//lfc_log_message(NAME, L_NOTICE, "adding to set %s script '%s'\n",
	//		action, name);

	FILE *fp;

	if (access(fc_script_set_actions, R_OK | W_OK | X_OK) != -1) {
		fp = fopen(fc_script_set_actions, "a");
		if (!fp) return -1;
	} else {
		fp = fopen(fc_script_set_actions, "w");
		if (!fp) return -1;

		fprintf(fp, "#!/bin/sh\n");

		if (chmod(fc_script_set_actions,
			strtol("0700", 0, 8)) < 0) {
			return -1;
		}
	}

#ifdef DUMMY_MODE
	fprintf(fp, "/bin/sh `pwd`/%s set %s %s '%s'\n", fc_script, action, name, value);
#else
	fprintf(fp, "/bin/sh %s set %s %s '%s'\n", fc_script, action, name, value);
#endif

	fclose(fp);

	return 0;
}

int external_set_action_execute()
{
	lfc_log_message(NAME, L_NOTICE, "executing set script\n");
	if ((uproc.pid = fork()) == -1) {
		return -1;
	}

	if (uproc.pid == 0) {
		/* child */

		const char *argv[3];
		int i = 0;
		argv[i++] = "/bin/sh";
		argv[i++] = fc_script_set_actions;
		argv[i++] = NULL;

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0)
		return -1;

	/* parent */
	int status;
	while (wait(&status) != uproc.pid) {
		DD("waiting for child to exit");
	}

	// TODO: add some kind of checks

	if (remove(fc_script_set_actions) != 0)
		return -1;

	return 0;
}

int external_object(char *action, char *name, char **instance)
{
	//lfc_log_message(NAME, L_NOTICE, "executing %s object '%s'\n",
	//		action, name);

	char *c = NULL;

	int pfds[2];
	if (pipe(pfds) < 0)
		return -1;

	if ((uproc.pid = fork()) == -1)
		goto error;

	if (uproc.pid == 0) {
		/* child */

		const char *argv[8];
		int i = 0;
		argv[i++] = "/bin/sh";
		argv[i++] = fc_script;
		argv[i++] = "--newline";
		argv[i++] = "--value";
		argv[i++] = action;
		argv[i++] = "object";
		argv[i++] = name;
		argv[i++] = NULL;

		close(pfds[0]);
		dup2(pfds[1], 1);
		close(pfds[1]);

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0)
		goto error;

	/* parent */
	close(pfds[1]);

	int status;
	while (wait(&status) != uproc.pid) {
		DD("waiting for child to exit");
	}

	if (strncasecmp("add", action, strlen("add")) != 0) goto done;

	char buffer[64];
	ssize_t rxed;
	int t;

	*instance = NULL;
	while ((rxed = read(pfds[0], buffer, sizeof(buffer))) > 0) {
		if (*instance)
			t = asprintf(&c, "%s%.*s", *instance, (int) rxed, buffer);
		else
			t = asprintf(&c, "%.*s", (int) rxed, buffer);

		if (t == -1) goto error;

		free(*instance);
		*instance = strdup(c);
		free(c);
	}

	if (!(*instance)) {
		goto done;
	}

	if (!strlen(*instance)) {
		FREE(*instance);
		goto error;
	}

	if (rxed < 0)
		goto error;

done:
	close(pfds[0]);
	return 0;

error:
	free(c);
	close(pfds[0]);
	return -1;
}

int external_simple(char *arg)
{
	lfc_log_message(NAME, L_NOTICE, "executing %s request\n", arg);

	if ((uproc.pid = fork()) == -1)
		return -1;

	if (uproc.pid == 0) {
		/* child */

		const char *argv[4];
		int i = 0;
		argv[i++] = "/bin/sh";
		argv[i++] = fc_script;
		argv[i++] = arg;
		argv[i++] = NULL;

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0)
		return -1;

	/* parent */
	int status;
	while (wait(&status) != uproc.pid) {
		DD("waiting for child to exit");
	}

	// TODO: add some kind of checks

	return 0;
}

int external_download(char *url, char *size)
{
	lfc_log_message(NAME, L_NOTICE, "executing download url '%s'\n", url);

	if ((uproc.pid = fork()) == -1)
		return -1;

	if (uproc.pid == 0) {
		/* child */

		const char *argv[8];
		int i = 0;
		argv[i++] = "/bin/sh";
		argv[i++] = fc_script;
		argv[i++] = "download";
		argv[i++] = "--url";
		argv[i++] = url;
		argv[i++] = "--size";
		argv[i++] = size;
		argv[i++] = NULL;

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0)
		return -1;

	/* parent */
	int status;
	while (wait(&status) != uproc.pid) {
		DD("waiting for child to exit");
	}

	if (WIFEXITED(status) && !WEXITSTATUS(status))
		return 0;
	else
		return 1;

	return 0;
}

int external_download_md5(char *url, char *size, char *md5)
{
	lfc_log_message(NAME, L_NOTICE, "executing download url '%s'\n", url);

	if ((uproc.pid = fork()) == -1)
		return -1;

	if (uproc.pid == 0) {
		/* child */

		const char *argv[10];
		int i = 0;
		argv[i++] = "/bin/sh";
		argv[i++] = fc_script;
		argv[i++] = "download";
		argv[i++] = "--url";
		argv[i++] = url;
		argv[i++] = "--size";
		argv[i++] = size;
		argv[i++] = "--md5";
		argv[i++] = md5;
		argv[i++] = NULL;

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0)
		return -1;

	/* parent */
	int status;
	while (wait(&status) != uproc.pid) {
		DD("waiting for child to exit");
	}

	if (WIFEXITED(status) && !WEXITSTATUS(status))
		return 0;
	else
		return 1;

	return 0;
}

