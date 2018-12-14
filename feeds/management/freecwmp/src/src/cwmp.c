/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
 */

#include <stdlib.h>
#include <string.h>

#include <libfreecwmp.h>
#include <libubox/uloop.h>

#include "cwmp.h"

#include "config.h"
#include "external.h"
#include "freecwmp.h"
#include "http.h"
#include "xml.h"

struct cwmp_internal *cwmp;

static void cwmp_periodic_interval_inform(struct uloop_timeout *timeout);
static void cwmp_periodic_time_inform(struct uloop_timeout *timeout);
static void cwmp_do_inform(struct uloop_timeout *timeout);

static struct uloop_timeout inform_timer = { .cb = cwmp_do_inform };
static struct uloop_timeout periodic_inform_interval_timer = { .cb = cwmp_periodic_interval_inform };
static struct uloop_timeout periodic_inform_time_timer = { .cb = cwmp_periodic_time_inform };

static void cwmp_periodic_interval_inform(struct uloop_timeout *timeout)
{
	if (cwmp->periodic_inform_enabled && cwmp->periodic_inform_interval) {
		uloop_timeout_set(&periodic_inform_interval_timer, cwmp->periodic_inform_interval * 1000);
		cwmp_add_event(PERIODIC, NULL);
	}

	if (cwmp->periodic_inform_enabled)
		cwmp_inform();
}

static void cwmp_periodic_time_inform(struct uloop_timeout *timeout)
{
	cwmp_add_event(PERIODIC, NULL);
	cwmp_inform();
}

static void cwmp_do_inform(struct uloop_timeout *timeout)
{
	cwmp_inform();
}

void cwmp_init(void)
{
	char *c = NULL;

	config_get_cwmp("InternetGatewayDevice.ManagementServer.PeriodicInformInterval", &c);
	if (c) {
		cwmp->periodic_inform_interval = atoi(c);
		uloop_timeout_set(&periodic_inform_interval_timer, cwmp->periodic_inform_interval * 1000);
		free(c);
		c = NULL;
	}

	config_get_cwmp("InternetGatewayDevice.ManagementServer.PeriodicInformEnable", &c);
	if (c) {
		cwmp->periodic_inform_enabled = atoi(c);
		free(c);
		c = NULL;
	}

	http_server_init();
}

int cwmp_inform(void)
{
	char *msg_in, *msg_out;
	msg_in = msg_out = NULL;

	clear_log(); 

	if (http_client_init()) {
		D("initializing http client failed\n");
		goto error;
	}

	if (xml_prepare_inform_message(&msg_out)) {
		D("xml message creating failed\n");
		goto error;
	}

	if (http_send_message(msg_out, &msg_in)) {
		D("sending http message failed\n");
		goto error;
	}

	if (msg_in && xml_parse_inform_response_message(msg_in)) {
		D("parse xml message from ACS failed\n");
		goto error;
	}

	cwmp->retry_count = 0;

	if (cwmp_handle_messages()) {
		D("handling xml message failed\n");
		goto error;
	}

	FREE(msg_in);
	FREE(msg_out);

	cwmp_remove_event(BOOTSTRAP);
	cwmp_remove_event(BOOT);
	cwmp_clear_notifications();
	http_client_exit();
	xml_exit();

	return 0;

error:
	FREE(msg_in);
	FREE(msg_out);

	http_client_exit();
	xml_exit();

	cwmp->retry_count++;
	if (cwmp->retry_count < 100) {
		uloop_timeout_set(&inform_timer, 10000 * cwmp->retry_count);
	} else {
		/* try every 20 minutes */
		uloop_timeout_set(&inform_timer, 1200000);
	}

	return -1;
}

int cwmp_handle_messages(void)
{
	char *msg_in, *msg_out;
	msg_in = msg_out = NULL;

	while (1) {
		FREE(msg_in);

		if (http_send_message(msg_out, &msg_in)) {
			D("sending http message failed\n");
			goto error;
		}

		if (!msg_in)
			break;

		FREE(msg_out);

		if (xml_handle_message(msg_in, &msg_out)) {
			D("xml handling message failed\n");
			goto error;
		}

		if (!msg_out) {
			D("acs response message is empty\n");
			goto error;
		}
	}

	FREE(msg_in);
	FREE(msg_out);

	return 0;

error:
	FREE(msg_in);
	FREE(msg_out);

	return -1;
}

void cwmp_connection_request(int code)
{
	cwmp_clear_events();
	cwmp_add_event(code, NULL);
	uloop_timeout_set(&inform_timer, 500);
}

void cwmp_add_event(int code, char *key)
{
	struct event *e = NULL;
	struct list_head *p;
	bool uniq = true;

	list_for_each(p, &cwmp->events) {
		e = list_entry(p, struct event, list);
		if (e->code == code) {
			uniq = false;
			break;
		}
	}

	if (uniq) {
		e = calloc(1, sizeof(*e));
		if (!e) return;

		list_add_tail(&e->list, &cwmp->events);
		e->code = code;
		e->key = key ? strdup(key) : NULL;
	}
}

void cwmp_remove_event(int code)
{
	struct event *n, *p;

	list_for_each_entry_safe(n, p, &cwmp->events, list) {
		if (code == n->code) {
			config_remove_event(lfc_str_event_code(code, true));
			FREE(n->key);
			list_del(&n->list);
			FREE(n);
		}
	}
}

void cwmp_clear_events(void)
{
	struct event *n, *p;

	list_for_each_entry_safe(n, p, &cwmp->events, list) {
		FREE(n->key);
		list_del(&n->list);
		FREE(n);
	}
}

void cwmp_add_notification(char *parameter, char *value)
{
	char *c = NULL;
	external_get_action("notification", parameter, &c);
	if (!c) return;

	struct notification *n = NULL;
	struct list_head *p;
	bool uniq = true;

	list_for_each(p, &cwmp->notifications) {
		n = list_entry(p, struct notification, list);
		if (!strcmp(n->parameter, parameter)) {
			free(n->value);
			n->value = strdup(value);
			uniq = false;
			break;
		}
	}

	if (uniq) {
		n = calloc(1, sizeof(*n));
		if (!n) return;

		list_add_tail(&n->list, &cwmp->notifications);
		n->parameter = strdup(parameter);
		n->value = strdup(value);
	}


	cwmp_add_event(VALUE_CHANGE, NULL);
	if (!strncmp(c, "2", 1)) {
		cwmp_inform();
	}
}

void cwmp_clear_notifications(void)
{
	struct notification *n, *p;

	list_for_each_entry_safe(n, p, &cwmp->notifications, list) {
		FREE(n->parameter);
		FREE(n->value);
		list_del(&n->list);
		FREE(n);
	}
}

int cwmp_set_parameter_write_handler(char *name, char *value)
{
	if((strcmp(name, "InternetGatewayDevice.ManagementServer.PeriodicInformEnable")) == 0) {
		cwmp->periodic_inform_enabled = atoi(value);
	}

	if((strcmp(name, "InternetGatewayDevice.ManagementServer.PeriodicInformInterval")) == 0) {
		cwmp->periodic_inform_interval = atoi(value);
		uloop_timeout_set(&periodic_inform_interval_timer, cwmp->periodic_inform_interval * 1000);
	}

	if((strcmp(name, "InternetGatewayDevice.ManagementServer.PeriodicInformTime")) == 0) {
		int interval = lfc_get_remaining_time(config->local->date_format, value);
		if (interval > 0)
			uloop_timeout_set(&periodic_inform_time_timer, interval * 1000);
	}

	return external_set_action_write("value", name, value);
}

