#ifndef __IN_PROMAC_C
#define __IN_PROMAC_C

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<fcntl.h>

#define STRLEN_12 12
#define STRLEN_17 17
#define MACLEN_6 6 
#define MAC_SEEK_4 4 
#define MAC_SEEK_40 40 
#define MAC_SEEK_46 46 

//#define MTD2 "/dev/mtd2" //factory block
//#define MTD2 "factory.bin" //factory block

struct stumac
{   
    unsigned char mac[MACLEN_6];
};
typedef struct stumac stumac_t;
typedef unsigned char uint8_t;

stumac_t *tomac(const char *pszStr,stumac_t *pmac);
stumac_t *macadd(stumac_t *pmac,uint8_t n); //0 <= n <=255
stumac_t *macadd_big(stumac_t *pmac,uint8_t n); //0 <= n <=255
stumac_t *macadd_big2(stumac_t *pmac,uint8_t n); //0 <= n <=255

int checkmac(const unsigned char* strMac);
/*
int writemac(FILE * pf,stumac_t *pstmac,long seek)
{
    if (NULL == pf || NULL == pstmac)
    {
        return -1;
    }
    if (-1 == fseek(pf,seek,SEEK_SET))
    {
        return -1;
    }
    if(1 != fwrite(pstmac->mac,MACLEN_6,1,pf))
    {
         return -1;
    }

    return 0;
}
void usage(void)
{
    fprintf(stderr, "usage: wrmac -m <MAC>\n");
}

void free_safy(void *p)
{
    if(NULL != p)
    {
        free(p);
    }
}
*/
#endif
