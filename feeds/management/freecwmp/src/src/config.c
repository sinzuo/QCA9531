/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>

#include <libfreecwmp.h>

#include "config.h"
#include "cwmp.h"

static bool first_run = true;
static struct uci_context *uci_ctx;
static struct uci_package *uci_freecwmp;

struct core_config *config;


static void config_free_local(void) {
	if (config->local) {
		FREE(config->local->ip);
		FREE(config->local->interface);
		FREE(config->local->port);
		FREE(config->local->date_format);
		FREE(config->local->username);
		FREE(config->local->password);
		FREE(config->local->ubus_socket);
	}
	cwmp_clear_events();
}

static int config_init_local(void)
{
	struct uci_section *s;
	struct uci_element *e1, *e2;

	uci_foreach_element(&uci_freecwmp->sections, e1) {
		s = uci_to_section(e1);
		if (strcmp(s->type, "local") == 0)
			goto section_found;
	}
	D("uci section local not found...\n");
	return -1;

section_found:
	config_free_local();

	uci_foreach_element(&s->options, e1) {
		if (!strcmp((uci_to_option(e1))->e.name, "interface")) {
			config->local->interface = strdup(uci_to_option(e1)->v.string);
			DD("freecwmp.@local[0].interface=%s\n", config->local->interface);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "port")) {
			if (!atoi((uci_to_option(e1))->v.string)) {
				D("in section local port has invalid value...\n");
				return -1;
			}
			config->local->port = strdup(uci_to_option(e1)->v.string);
			DD("freecwmp.@local[0].port=%s\n", config->local->port);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "date_format")) {
			config->local->date_format = strdup(uci_to_option(e1)->v.string);
			DD("freecwmp.@local[0].date_format=%s\n", config->local->date_format);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "username")) {
			config->local->username = strdup(uci_to_option(e1)->v.string);
			DD("freecwmp.@local[0].username=%s\n", config->local->username);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "password")) {
			config->local->password = strdup(uci_to_option(e1)->v.string);
			DD("freecwmp.@local[0].password=%s\n", config->local->password);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "ubus_socket")) {
			config->local->ubus_socket = strdup(uci_to_option(e1)->v.string);
			DD("freecwmp.@local[0].ubus_socket=%s\n", config->local->ubus_socket);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "event") &&
		    (uci_to_option(e1))->type == UCI_TYPE_LIST) {
			uci_foreach_element(&((uci_to_option(e1))->v.list), e2) {
				if (e2 && e2->name) {
					cwmp_add_event(lfc_int_event_code(e2->name), NULL);
					DD("freecwmp.@local[0].event=%s\n", e2->name);
				}
			}
		}

next:
		;
	}

	if (!config->local->interface) {
		D("in local you must define interface\n");
		return -1;
	}

	if (!config->local->port) {
		D("in local you must define port\n");
		return -1;
	}

	if (!config->local->date_format) {
		D("in local you must define date_format\n");
		return -1;
	}

	if (!config->local->ubus_socket) {
		D("in local you must define ubus_socket\n");
		return -1;
	}

	return 0;
}

static void config_free_acs(void) {
	if (config->acs) {
		FREE(config->acs->scheme);
		FREE(config->acs->username);
		FREE(config->acs->password);
		FREE(config->acs->hostname);
		FREE(config->acs->port);
		FREE(config->acs->path);
		FREE(config->acs->ssl_cert);
		FREE(config->acs->ssl_cacert);
	}
}

static int config_init_acs(void)
{
	struct uci_section *s;
	struct uci_element *e;

	uci_foreach_element(&uci_freecwmp->sections, e) {
		s = uci_to_section(e);
		if (strcmp(s->type, "acs") == 0)
			goto section_found;
	}
	D("uci section acs not found...\n");
	return -1;

section_found:
	config_free_acs();

	uci_foreach_element(&s->options, e) {
		if (!strcmp((uci_to_option(e))->e.name, "scheme")) {
			/* TODO: ok, it's late and this does what i need */
			bool valid = false;

			if (!(strncmp((uci_to_option(e))->v.string, "http", 5)))
				valid = true;

			if (!(strncmp((uci_to_option(e))->v.string, "https", 6)))
				valid = true;

			if (!valid) {
				D("in section acs scheme must be either http or https...\n");
				return -1;
			}

			config->acs->scheme = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@acs[0].scheme=%s\n", config->acs->scheme);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "username")) {
			config->acs->username = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@acs[0].username=%s\n", config->acs->username);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "password")) {
			config->acs->password = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@acs[0].password=%s\n", config->acs->password);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "hostname")) {
			config->acs->hostname = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@acs[0].hostname=%s\n", config->acs->hostname);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "port")) {
			if (!atoi((uci_to_option(e))->v.string)) {
				D("in section acs port has invalid value...\n");
				return -1;
			}
			config->acs->port = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@acs[0].port=%s\n", config->acs->port);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "path")) {
			config->acs->path = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@acs[0].path=%s\n", config->acs->path);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "ssl_cert")) {
			config->acs->ssl_cert = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@acs[0].ssl_cert=%s\n", config->acs->ssl_cert);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "ssl_cacert")) {
			config->acs->ssl_cacert = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@acs[0].ssl_cacert=%s\n", config->acs->ssl_cacert);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "ssl_verify")) {
			if (!strcmp((uci_to_option(e))->v.string, "enabled")) {
				config->acs->ssl_verify = true;
			} else {
				config->acs->ssl_verify = false;
			}
			DD("freecwmp.@acs[0].ssl_verify=%d\n", config->acs->ssl_verify);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "auth_digest")) {
			if (!strcmp((uci_to_option(e))->v.string, "enabled")) {
				config->acs->auth_digest = true;
			} else {
				config->acs->auth_digest = false;
			}
			DD("freecwmp.@acs[0].auth_digest=%d\n", config->acs->auth_digest);
			goto next;
		}

next:
		;
	}

	if (!config->acs->scheme) {
		D("in acs you must define scheme\n");
		return -1;
	}

	if (!config->acs->username) {
		D("in acs you must define username\n");
		return -1;
	}

	if (!config->acs->password) {
		D("in acs you must define password\n");
		return -1;
	}

	if (!config->acs->hostname) {
		D("in acs you must define hostname\n");
		return -1;
	}

	if (!config->acs->port) {
		D("in acs you must define port\n");
		return -1;
	}

	if (!config->acs->path) {
		D("in acs you must define path\n");
		return -1;
	}

	return 0;
}

static void config_free_device(void)
{
	if (config->device) {
		FREE(config->device->manufacturer);
		FREE(config->device->oui);
		FREE(config->device->product_class);
		FREE(config->device->serial_number);
		FREE(config->device->hardware_version);
		FREE(config->device->software_version);
	}
}

static int config_init_device(void)
{
	struct uci_section *s;
	struct uci_element *e;

	uci_foreach_element(&uci_freecwmp->sections, e) {
		s = uci_to_section(e);
		if (strcmp(s->type, "device") == 0)
			goto section_found;
	}
	D("uci section device not found...\n");
	return -1;

section_found:
	config_free_device();

	uci_foreach_element(&s->options, e) {
		if (!strcmp((uci_to_option(e))->e.name, "manufacturer")) {
			config->device->manufacturer = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@device[0].manufacturer=%s\n", config->device->manufacturer);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "oui")) {
			config->device->oui = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@device[0].oui=%s\n", config->device->oui);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "product_class")) {
			config->device->product_class = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@device[0].product_class=%s\n", config->device->product_class);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "serial_number")) {
			config->device->serial_number = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@device[0].serial_number=%s\n", config->device->serial_number);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "hardware_version")) {
			config->device->hardware_version = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@device[0].hardware_version=%s\n", config->device->hardware_version);
			goto next;
		}

		if (!strcmp((uci_to_option(e))->e.name, "software_version")) {
			config->device->software_version = strdup(uci_to_option(e)->v.string);
			DD("freecwmp.@device[0].software_version=%s\n", config->device->software_version);
			goto next;
		}

next:
		;
	}

	if (!config->device->product_class) {
		D("in device you must define product_class\n");
		return -1;
	}

	if (!config->device->serial_number) {
		D("in device you must define serial_number\n");
		return -1;
	}

	if (!config->device->hardware_version) {
		D("in device you must define hardware_version\n");
		return -1;
	}

	if (!config->device->software_version) {
		D("in device you must define software_version\n");
		return -1;
	}

	return 0;
}

int config_get_cwmp(char *parameter, char **value)
{
	struct uci_section *s;
	struct uci_element *e1, *e2;

	uci_foreach_element(&uci_freecwmp->sections, e1) {
		s = uci_to_section(e1);

		if (strcmp(s->type, "cwmp"))
			continue;

		bool found = false;
		uci_foreach_element(&s->options, e2) {
			if (!strcmp((uci_to_option(e2))->e.name, "parameter") &&
			    !strcmp((uci_to_option(e2))->v.string, parameter)) {
				found = true;
				break;
			}
		}

		uci_foreach_element(&s->options, e2) {
			if (!strcmp((uci_to_option(e2))->e.name, "value") && found) {
				*value = strdup(uci_to_option(e2)->v.string);
				return 0;
			}
		}

		if (found) return 1;
	}

	return 2;
}

int config_remove_event(char *event)
{
	struct uci_ptr ptr;

	char *c;

	if (asprintf(&c, "freecwmp.@local[0].event=%s", event) == -1)
		return -1;

	if (uci_lookup_ptr(uci_ctx, &ptr, c, true)) {
		FREE(c);
		return 1;
	}

	FREE(c);

	uci_del_list(uci_ctx, &ptr);
	if (uci_save(uci_ctx, ptr.p)) return 1;
	if (uci_commit(uci_ctx, &ptr.p, true)) return 1;

	return 0;
}

static struct uci_package *
config_init_package(const char *c)
{
	struct uci_context *ctx = uci_ctx;
	struct uci_package *p = NULL;

	if (first_run) {
		config = calloc(1, sizeof(struct core_config));
		if (!config) goto error;

		config->acs = calloc(1, sizeof(struct acs));
		if (!config->acs) goto error;

		config->device = calloc(1, sizeof(struct device));
		if (!config->device) goto error;

		config->local = calloc(1, sizeof(struct local));
		if (!config->local) goto error;
	}

	if (!ctx) {
		ctx = uci_alloc_context();
		if (!ctx) goto error;
		uci_ctx = ctx;

#ifdef DUMMY_MODE
		uci_set_confdir(ctx, "./ext/openwrt/config");
		uci_set_savedir(ctx, "./ext/tmp");
#endif

	} else {
		p = uci_lookup_package(ctx, c);
		if (p)
			uci_unload(ctx, p);
	}

	if (uci_load(ctx, c, &p)) {
		uci_free_context(ctx);
		return NULL;
	}

	return p;

error:
	if (config) {
		config_free_acs();
		FREE(config->acs);
		config_free_device();
		FREE(config->device);
		config_free_local();
		FREE(config->local);
		FREE(config);
		uci_free_context(uci_ctx);
	}

	return NULL;
}

void config_exit(void)
{
		config_free_acs();
		FREE(config->acs);
		config_free_device();
		FREE(config->device);
		config_free_local();
		FREE(config->local);
		FREE(config);
		uci_free_context(uci_ctx);
}

void config_load(void)
{
	uci_freecwmp = config_init_package("freecwmp");
	if (!uci_freecwmp) goto error;

	if (config_init_local()) goto error;
	if (config_init_acs()) goto error;
	if (config_init_device()) goto error;

	first_run = false;
	return;

error:
	lfc_log_message(NAME, L_CRIT, "configuration (re)loading failed\n");
	D("configuration (re)loading failed\n");
	exit(EXIT_FAILURE);
}

