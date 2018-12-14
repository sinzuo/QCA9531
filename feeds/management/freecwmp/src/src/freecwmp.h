/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
 */

#ifndef _FREECWMP_FREECWMP_H__
#define _FREECWMP_FREECWMP_H__

#include<stdio.h>
#include<stdarg.h>

#define NAME	"freecwmpd"

//added by pixiaocong in 2017-05-11
#define TIME_STAMP_SIZE 32  
#define LOG_FILE "/tmp/freecwmp.log"
#define LOG_FILE_SIZE  1 * 1024 * 1024 
/*
 *If the log file is greater than 1M,clear the log file,call clean_log function. 
*/
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
//added ended 

#define FREE(x) do { free(x); x = NULL; } while (0);

#ifdef DEBUG
//#define D(format, ...) fprintf(stderr, "%s(%d): " format, __func__, __LINE__, ## __VA_ARGS__)
#define D(format,...) \
        do { \
	   get_current_time(); \
	   write_log(__FILENAME__,__LINE__,format, ## __VA_ARGS__); \
        }while(0) 
#else
#define D(format, ...) no_debug(0, format, ## __VA_ARGS__)
#endif

#ifdef DEVEL
//#define DD(format, ...) fprintf(stderr, "%s(%d):: " format, __func__, __LINE__, ## __VA_ARGS__)
#define DD(format,...) \
        do { \
	   get_current_time(); \
	   write_log(__FILENAME__,__LINE__,format, ## __VA_ARGS__); \
        }while(0) 
//#define DDF(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#define DDF(format,...) \
        do { \
	   get_current_time(); \
	   write_log(__FILENAME__,__LINE__,format, ## __VA_ARGS__); \
        }while(0) 
#else
#define DD(format, ...) no_debug(0, format, ## __VA_ARGS__)
#define DDF(format, ...) no_debug(0, format, ## __VA_ARGS__)
#endif

extern void write_log(const char *fileName,int lineNumber,const char *fmt,...);
extern void get_current_time(void);
extern void clear_log(void);

static inline void no_debug(int level, const char *fmt, ...)
{
}

void freecwmp_reload(void);

#endif

