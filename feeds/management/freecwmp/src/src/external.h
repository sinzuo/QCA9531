/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#ifndef _FREECWMP_EXTERNAL_H__
#define _FREECWMP_EXTERNAL_H__

int external_get_action(char *action, char *name, char **value);
int external_get_inform(char *name, char **value);
int external_set_action_write(char *action, char *name, char *value);
int external_set_action_execute();
int external_object(char *action, char *name, char **instance);
int external_simple(char *arg);
int external_download(char *url, char *size);
int external_download_md5(char *url, char *size,char *md5);

#endif

