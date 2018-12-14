/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#ifndef _FREECWMP_XML_H__
#define _FREECWMP_XML_H__

#include <microxml.h>

void xml_exit(void);
char* urldecode(char *input);
int xml_prepare_inform_message(char **msg_out);
int xml_parse_inform_response_message(char *msg_in);
int xml_handle_message(char *msg_in, char **msg_out);

#endif

