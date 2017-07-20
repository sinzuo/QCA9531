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

#include "config.h"

#define FREE(x) do { free(x); x = NULL; } while (0);

static bool first_run = true;
static struct uci_context *uci_ctx;
static struct uci_package *uci_probe;

struct core_config *config;

static void config_free_report(void) {
	if (config->report) {
		FREE(config->report->enable);
		FREE(config->report->port);
		FREE(config->report->hostname);
		FREE(config->report->proto);
		FREE(config->report->interval);
		FREE(config->report->threshold);
	}
}

static int config_init_report(void)
{
	struct uci_section *s;
	struct uci_element *e1, *e2;

	uci_foreach_element(&uci_probe->sections, e1) {
		s = uci_to_section(e1);
		if (strcmp(s->type, "report") == 0)
			goto section_found;
	}
	printf("uci section report not found...\n");
	return -1;

section_found:
	config_free_report();

	uci_foreach_element(&s->options, e1) {
		if (!strcmp((uci_to_option(e1))->e.name, "enable")) {
			config->report->enable = strdup(uci_to_option(e1)->v.string);
			printf("enable=%s\n", config->report->enable);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "port")) {
			if (!atoi((uci_to_option(e1))->v.string)) {
				printf("in section reprot port has invalid value...\n");
				return -1;
			}
			config->report->port = strdup(uci_to_option(e1)->v.string);
			printf("port=%s\n", config->report->port);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "hostname")) {
			config->report->hostname = strdup(uci_to_option(e1)->v.string);
			printf("hostname=%s\n", config->report->hostname);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "proto")) {
			config->report->proto = strdup(uci_to_option(e1)->v.string);
			printf("proto=%s\n", config->report->proto);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "interval")) {
			if (!atoi((uci_to_option(e1))->v.string)) {
				printf("in section reprot interval has invalid value...\n");
				return -1;
			}
			config->report->interval = strdup(uci_to_option(e1)->v.string);
			printf("interval=%s\n", config->report->interval);
			goto next;
		}

		if (!strcmp((uci_to_option(e1))->e.name, "threshold")) {
			if (!atoi((uci_to_option(e1))->v.string)) {
				printf("in section reprot threshold has invalid value...\n");
				return -1;
			}
			config->report->threshold = strdup(uci_to_option(e1)->v.string);
			printf("threshold=%s\n", config->report->threshold);
			goto next;
		}

next:
		;
	}

	if (!config->report->port) {
		printf("in report you must define port\n");
		return -1;
	}

	if (!config->report->hostname) {
		printf("in report you must define hostname\n");
		return -1;
	}

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

		config->report = calloc(1, sizeof(struct report));
		if (!config->report) goto error;
	}

	if (!ctx) {
		ctx = uci_alloc_context();
		if (!ctx) goto error;
		uci_ctx = ctx;
/*
#ifdef DUMMY_MODE
		uci_set_confdir(ctx, "./ext/openwrt/config");
		uci_set_savedir(ctx, "./ext/tmp");
#endif
*/

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
		config_free_report();
		FREE(config->report);
		FREE(config);
		uci_free_context(uci_ctx);
	}

	return NULL;
}

void config_exit(void)
{
	config_free_report();
	FREE(config->report);
	FREE(config);
	uci_free_context(uci_ctx);
}

void config_load(void)
{
	uci_probe = config_init_package("probe");
	if (!uci_probe) goto error;

	if (config_init_report()) goto error;

	first_run = false;
	return;

error:
	printf("configuration (re)loading failed\n");
	exit(EXIT_FAILURE);
}

