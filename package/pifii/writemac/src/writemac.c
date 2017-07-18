#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "promac.h"
//#include <linux/autoconf.h>

//#define MTD_FACTORY     "/dev/mtd4"
//#define MTD_FACTORY     "./art.bin"

#define WAN_OFFSET      0x0
#define LAN_OFFSET      0x6
#define WLAN_OFFSET     0x1002
#define WLAN2_OFFSET    0x5006

#define MACADDR_LEN     6
#define WIFIRF_LEN  512

#define MEMGETINFO  _IOR('M', 1, struct mtd_info_user)
#define MEMERASE    _IOW('M', 2, struct erase_info_user)
#define MEMUNLOCK   _IOW('M', 6, struct erase_info_user)

char MTD_FACTORY[16] = "/dev/mtd4";

struct erase_info_user
{
    unsigned int start;
    unsigned int length;
};

struct mtd_info_user
{
    unsigned char type;
    unsigned int flags;
    unsigned int size;
    unsigned int erasesize;
    unsigned int oobblock;
    unsigned int oobsize;
    unsigned int ecctype;
    unsigned int eccsize;
};
/*
int mtd_read(char *side)
{
    int fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
    int i = 0;
    unsigned char mac_addr[MACADDR_LEN];

    if(fd < 0)
    {
        printf("Could not open mtd device: %s\n", MTD_FACTORY);
        return -1;
    }

    if (!strcmp(side, "wlan"))
        lseek(fd, WLAN_OFFSET, SEEK_SET);
    else if (!strcmp(side, "wan"))
        lseek(fd, WAN_OFFSET, SEEK_SET);
    else
        lseek(fd, LAN_OFFSET, SEEK_SET);
    if(read(fd, mac_addr, MACADDR_LEN) != MACADDR_LEN)
    {
        printf("read() failed\n");
        close(fd);
        return -1;
    }
    for (i = 0; i < MACADDR_LEN; i++)
    {
        printf("%02X", mac_addr[i]);
        if (i < MACADDR_LEN-1)
            printf(":");
        else
            printf("\n");
    }
    close(fd);

    return 0;
}
*/

int mtd_write( stumac_t *pstmac)
{
    int sz = 0;
    int i;
    struct mtd_info_user mtdInfo;
    struct erase_info_user mtdEraseInfo;
    int fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
    unsigned char *buf, *ptr;
    if(fd < 0)
    {
        fprintf(stderr, "Could not open mtd device: %s\n", MTD_FACTORY);
        return -1;
    }
    if(ioctl(fd, MEMGETINFO, &mtdInfo))
    {
        fprintf(stderr, "Could not get MTD device info from %s\n", MTD_FACTORY);
        close(fd);
        return -1;
    }
    mtdEraseInfo.length = sz = mtdInfo.erasesize;
    buf = (unsigned char *)malloc(sz);
	if(NULL == buf){
		printf("Allocate memory for sz failed.\n");
		close(fd);
		return -1;        
	}
	if(read(fd, buf, sz) != sz){
        fprintf(stderr, "read() %s failed\n", MTD_FACTORY);
        goto write_fail;
    }
    mtdEraseInfo.start = 0x0;
    for (mtdEraseInfo.start; mtdEraseInfo.start < mtdInfo.size; mtdEraseInfo.start += mtdInfo.erasesize)
    {
        ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
        if(ioctl(fd, MEMERASE, &mtdEraseInfo))
        {
            fprintf(stderr, "Failed to erase block on %s at 0x%x\n", MTD_FACTORY, mtdEraseInfo.start);
            goto write_fail;
        }
    }
    //write wan mac
    ptr = buf + WAN_OFFSET;
    memcpy(ptr,pstmac->mac,MACLEN_6);

    //write lan mac, lan mac = wan mac + 1
    macadd(pstmac,1);
    ptr = buf + LAN_OFFSET;
    memcpy(ptr,pstmac->mac,MACLEN_6);

    //write wlan mac, wlan mac = lan mac + 1
    macadd(pstmac,1);
    ptr = buf + WLAN_OFFSET;
    memcpy(ptr,pstmac->mac,MACLEN_6);

    //write wlan2 mac, wlan2 mac = waln mac + 1
    macadd(pstmac,1);
    ptr = buf + WLAN2_OFFSET;
    memcpy(ptr,pstmac->mac,MACLEN_6);

    /*
    if (!strcmp(side, "wlan"))
        ptr = buf + WLAN_OFFSET;
    else if (!strcmp(side, "wan"))
        ptr = buf + WAN_OFFSET;
    else
        ptr = buf + LAN_OFFSET;
    for (i = 0; i < MACADDR_LEN; i++, ptr++)
        *ptr = strtoul(value[i], NULL, 16);
    */
    lseek(fd, 0, SEEK_SET);
    if (write(fd, buf, sz) != sz)
    {
        fprintf(stderr, "write() %s failed\n", MTD_FACTORY);
        goto write_fail;
    }

    close(fd);
        free(buf);
    return 0;
write_fail:
    close(fd);
    free(buf);
    return -1;
}

char *get_mtd(char *name,char *buf)
{
    if( NULL == name || NULL == buf)
    {
        return NULL;
    }
    char line[64]="\0";
    char *mtd = NULL;
    FILE * fp = fopen("/proc/mtd","r");
    //FILE * fp = fopen("./mtd","r");
    if (NULL == fp)
    {
       return NULL;
    }
    while(NULL != fgets(line,sizeof(line),fp))
    {
        if(NULL != strstr(line,name) && NULL != strtok(line,":"))
        {
            strcpy(buf,"/dev/");
            strncpy(buf+5,line,strlen(line)+1);
            fclose(fp);
            return buf;
        }
    }
    fclose(fp);
    return NULL;
}

void usage(char **str)
{
    printf("How to use:\n");
    printf("\twrite:  %s m <mac> \n", str[0]);
}

int main(int argc,char **argv)
{
    char op;

    if (3 != argc)
        goto CmdFail;

    op = *(argv[1]);
    switch (op)
    {
        case 'm':
	    if ( NULL == get_mtd("art",MTD_FACTORY))
            {
                printf("get mtd for art failed! ");
                goto Fail;
            }
            if (!checkmac(argv[2]))
            {
                printf("invalid mac:%s\n",argv[2]);
                goto Fail;
            }
            stumac_t stmac;
            bzero(&stmac,sizeof(stumac_t));
            tomac(argv[2],&stmac);
            /*
            printf("mac\n");
            int i ; 
            for(i = 0;i<MACLEN_6;i++)
            {
                printf("%X",stmac.mac[i]);
            }
            putchar('\n');
            */
            if (mtd_write(&stmac) < 0)
            {
                goto Fail;
            }
            break;
        default:
            goto CmdFail;
    }

    printf("write mac successfully\n");
    return 0;
CmdFail:
    usage(argv);
    return -1;
Fail:
    printf("write mac failed\n");
    return -1;
}

