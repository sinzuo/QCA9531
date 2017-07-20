#ifndef  __LOG_H__
#define  __LOG_H__

#include<stdio.h>
#include<stdarg.h>

#define TIME_STAMP_SIZE 32  
#define LOG_FILE "/tmp/clientprobe.log"
#define LOG_FILE_SIZE  1 * 1024 * 1024 
/*
 *If the log file is greater than 1M,clear the log file,call clean_log function. 
*/
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)

#ifdef DEBUG
#define D(format,...) \
        do { \
           get_current_time(); \
           write_log(__FILENAME__,__LINE__,format, ## __VA_ARGS__); \
        }while(0) 
#else
#define D(format, ...) no_debug(0, format, ## __VA_ARGS__)
#endif

extern void write_log(const char *fileName,int lineNumber,const char *fmt,...);
extern void get_current_time(void);
extern void clear_log(void);

static inline void no_debug(int level, const char *fmt, ...)
{
}
#endif
