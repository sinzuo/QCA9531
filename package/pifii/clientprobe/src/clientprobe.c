#include <stdio.h>
#include <libubox/uloop.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "config.h"
#include "probedata.h" 
#include "log.h"
#include "probesource.h"

#define DEFAULT_INTERVAL 6
#define PROBE_BUF 256 * 8 
#define ONE_PACK_SIZE 8 

struct uloop_timeout g_send_data_timer;
int g_interval = DEFAULT_INTERVAL;
int g_socket = -1;
int g_client_total = 0;
int8_t g_client_dbm = 0;
uint8_t g_client_char = 0;

struct sockaddr_in g_addr;
struct probe_device g_pdev;

static char * getip(const char *hostname,char *ipbuf)
{
    if(NULL == hostname || NULL == ipbuf)
    {
        return NULL;
    }
    struct hostent *host;
    host = gethostbyname(hostname);
    if(NULL == host)
    {
        D("can not get host by hostname\n");
        return NULL;
    }
    sprintf(ipbuf,"%s",inet_ntoa(*((struct in_addr*)host->h_addr)));
    return ipbuf;
}

static int socket_udp_init(int port,char *ip)
{
    if(NULL == ip)
    {
        return -1;
    }
    int sock;
    if( (sock=socket(AF_INET, SOCK_DGRAM, 0)) <0)
    {
        perror("socket");
        return -1;
    }
    g_addr.sin_family = AF_INET;
    g_addr.sin_port = htons(port);
    g_addr.sin_addr.s_addr = inet_addr(ip);
    if (g_addr.sin_addr.s_addr == INADDR_NONE)
    {
        D("Incorrect ip address!");
        close(sock);
        return -1;
    }
    g_socket = sock;
    return 0;
}

static uint32_t remove_duplicate(uint8_t *src,uint32_t size,uint8_t *buf,uint32_t bufsize)
{
    if(NULL == src || NULL == buf || size > bufsize)
    {
        return 0;
    }
    uint32_t i=0; 
    uint32_t j=0;
    uint32_t r=0;
    uint32_t dup=0;
    int32_t dbm=-100;
    uint8_t *p=NULL;
    uint32_t len = ONE_PACK_SIZE;
    for(i=0;i<size;i=i + ONE_PACK_SIZE)
    {
        dup=0; 
        for(j=0;j<r;j=j + len)
        {
           if(0 == strncmp(src+i,buf+j,6)) 
           {
               dup=1;
               break;
           }
        }  
        if(0 == dup)
        {
            p=src+i;
            dbm= p[6] - 95;
            if(dbm >= g_pdev.threshold)
            {
                p[6]= p[6] - 95;
                memcpy(buf+r,p,len);
                r=r+len;
                D(" %02X:%02X:%02X:%02X:%02X:%02X, %d, %d",p[0],p[1],p[2],p[3],p[4],p[5],dbm,p[7]);
            }
        }
    }
    return r;
}
static void probe_data_sender(struct uloop_timeout *timeout)
{
    clear_log();
    int8_t dbm = 0;
    int total = 0;
    int sendtotal = 0;

    uint8_t mactable[PROBE_BUF];
    uint8_t senddata[PROBE_BUF];
    uint8_t *data;
    uint32_t rc = PROBE_BUF;
    uint32_t size = 0;
    uint32_t i = 0;
    /*
    FILE *fp;
    if(NULL == (fp=fopen("/tmp/mac","r")))
    {   
        D("/tmp/mac file cannot be opened");
	goto done;
    }
    size=fread(mactable,1,rc,fp);
    fclose(fp);
    */
    if(-1 == probe_source_status())
    {
        probe_source_init();
    }
    size=probe_source_read(mactable,rc);
    total = size / ONE_PACK_SIZE;
    if (0 < total)
    {
        dbm = *(mactable + 6) - 95; 
    	if (g_client_dbm == dbm && g_client_char == *mactable)
     	{
            D(" not found new data");
            goto done;
    	}
        g_client_dbm = dbm; 
        g_client_char = *mactable; 
        uint32_t sendsize=remove_duplicate(mactable,size,senddata,PROBE_BUF); 
        uint32_t sendlen = sizeof(struct probe_head)+ sendsize;
        uint8_t *sendbuf = (uint8_t *)malloc(sendlen);
        if (NULL != sendbuf)
        {
            bzero(sendbuf,sendlen); 
            struct probe_head *pph=(struct probe_head*)sendbuf;
            uint8_t *ppack=(sendbuf + sizeof(struct probe_head));
            pph->ver=2; //pre:1,now:2
            pph->type='p';
            memcpy(pph->apmac,g_pdev.device_id,MAC_ADDR_LEN);
            pph->size=sendsize;
            memcpy(ppack,senddata,sendsize);
            sendto(g_socket,sendbuf,sendlen,0,(struct sockaddr *)&g_addr,sizeof(struct sockaddr));
            free(sendbuf);
            sendbuf = NULL;
            D(" send mac total: %d",sendsize/ONE_PACK_SIZE);
        }
    }
done:
    uloop_timeout_set(timeout,g_interval * 1000); 
    return ;
}

int main(int argc,const char**argv)
{
    struct option long_opts[] = {
		{"foreground", no_argument, NULL, 'f'},
		{NULL, 0, NULL, 0}
	};
    int c;
    int foreground = 0; 
    while (1) 
    {
	c = getopt_long(argc, argv, "f", long_opts, NULL);
	if (c == EOF)
            break;
        switch (c) 
        {
            case 'f':
                 foreground = 1;
                 break;
            default:
                 fprintf(stderr, "error while parsing options\n");
                 exit(EXIT_FAILURE);
	}
    }
    if(0 == foreground) 
    {
        pid_t pid, sid;
        pid = fork();
        if (pid < 0)
        	exit(EXIT_FAILURE);
        if (pid > 0)
        	exit(EXIT_SUCCESS);
        
        sid = setsid();
        if (sid < 0) {
        	D("setsid() returned error\n");
        	exit(EXIT_FAILURE);
        }
        
        char *directory = ".";
        if ((chdir(directory)) < 0) {
            D("chdir() returned error\n");
            exit(EXIT_FAILURE);
        }
        
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    } 
    config_load();
    if(0 == strcmp(config->report->enable,"0")) 
    {
	D("/etc/config/probe --> enable:0");
        goto done;
    }
    if ( NULL == get_device_id(g_pdev.device_id))
    {
        D("get device mac/id failed");
        goto error;
    }
    if(config->report->interval)
    {
       int i = atoi(config->report->interval);
       if (5 <= i)
       {
           g_interval =  i;
       }
    }
    g_pdev.threshold = -100;
    if(config->report->threshold)
    {
        g_pdev.threshold = atoi(config->report->threshold);
        g_pdev.threshold = g_pdev.threshold == 0 ? -100 : g_pdev.threshold;
        g_pdev.threshold = g_pdev.threshold > 0 ? 0 - g_pdev.threshold : g_pdev.threshold;
    }
    char ipbuf[32] = "\0";

serip:
    bzero(ipbuf,32);    
    if(NULL == getip(config->report->hostname,ipbuf))
    {
        D("getip failed");
        sleep(20);
	goto serip;
    }
    if(0 != socket_udp_init(atoi(config->report->port),ipbuf))
    {
        D("create socket failed");
        sleep(20);
	goto serip;
    }
    probe_source_init();
    uloop_init();
    g_send_data_timer.cb=probe_data_sender;
    uloop_timeout_set(&g_send_data_timer,g_interval * 1000); 
    uloop_run();
    uloop_done();
    probe_source_destroy();

done:
    config_exit();
    return 0;

error:
    config_exit();
    return -1;
}
