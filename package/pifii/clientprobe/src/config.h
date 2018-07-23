/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#ifndef _PROBE_CONFIG_H__
#define _PROBE_CONFIG_H__

#include <uci.h>

void config_exit(void);
void config_load(void);
//int config_get_cwmp(char *parameter, char **value);
//int config_remove_event(char *event);

struct report {
	char *enable;
	char *hostname;
	char *port;
	char *proto;
	char *interval;
	char *threshold;
};

struct core_config {
	struct report *report;
};

extern struct core_config *config;

#endif

