#include <sys/stat.h>
#include "log.h"

char g_pszTimeStamp[TIME_STAMP_SIZE];

void write_log(const char *fileName,int lineNumber,const char *fmt,...)
{   
    FILE *fp = fopen(LOG_FILE,"a+");
    va_list va_l;
    va_start(va_l,fmt);
    fprintf(fp,">>%s[%s:%d]",g_pszTimeStamp,fileName,lineNumber);
    vfprintf(fp,fmt,va_l);
    fprintf(fp,"\n");
    va_end(va_l);
    fflush(fp);
    fclose(fp);
}

void get_current_time()
{
    time_t timep;
    struct tm *ptm;
    time(&timep);
    ptm = localtime(&timep);
    memset(g_pszTimeStamp,'\0',TIME_STAMP_SIZE);
    strftime(g_pszTimeStamp,TIME_STAMP_SIZE,"%Y-%m-%d %H:%M:%S",ptm);
}

void clear_log(void)
{
    unsigned long filesize = 0;
    struct stat statbuff;
    if(0 == stat(LOG_FILE, &statbuff))
    {
        filesize = statbuff.st_size;
    }
    if(LOG_FILE_SIZE < filesize)
    {
        FILE * fp = NULL;
        if( NULL != (fp = fopen(LOG_FILE,"w")))
        {
            fprintf(fp,"\n");
            fclose(fp);
        }
    }
}
