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

#define DEFAULT_INTERVAL 6

struct uloop_timeout g_send_data_timer;
int g_interval = DEFAULT_INTERVAL;
int g_socket = -1;
int g_client_total = 0;
int g_client_dbm = 0;
uint8_t g_client_char = 0;

struct sockaddr_in g_addr;
struct probe_pack g_pp;
struct probe_device g_pdev;

char * getip(const char *hostname,char *ipbuf)
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

int socket_udp_init(int port,char *ip)
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

void read_mac(){	
    FILE *fp;
    char mactable[1400];
    char *data;	
    unsigned rc	= 1400;
    unsigned int size;
    int i = 0;
    data=mactable;
    if(NULL == (fp=fopen("/tmp/mac","r")))
    {
        printf("file cannot be opened\n");
        return ;
    }
    size=fread(mactable,1,rc,fp);
    for(i=0;i<size;i=i+7)
    {
        data = mactable + i;
          
    }
    fclose(fp);
}

static void probe_data_sender(struct uloop_timeout *timeout)
{
    clear_log();
    strcpy(g_pdev.ssid,"wifi-free");
    strcpy(g_pdev.channel,"auto");
    exe_shell("uci -q get wireless.@wifi-iface[0].ssid",g_pdev.ssid,64);
    exe_shell("uci -q get wireless.wifi0.channel",g_pdev.channel,5);
    
    int dbm = 0;
    int total = 0;
    int sendtotal = 0;

    FILE *fp;
    uint8_t mactable[1400];
    uint8_t *data;
    uint32_t rc = 1400;
    uint32_t size = 0;
    int i = 0;
    data=mactable;
    if(NULL == (fp=fopen("/tmp/mac","r")))
    {   
        D("/tmp/mac file cannot be opened");
	goto done;
    }
    size=fread(mactable,1,rc,fp);
    fclose(fp);
    total = size / 7;
    if (0 < total)
    {
        dbm = *(mactable + 6) - 95; 
    	if (g_client_dbm == dbm && g_client_char == *mactable)
     	{
            goto done;
    	}
        g_client_dbm = dbm; 
        g_client_char = *mactable; 
        for(i=0;i<size;i=i+7)
        {   
            data = mactable + i;
            dbm = *(data + 6) - 95; 
            //D("dbm:%d",dbm);
            if(dbm < g_pdev.threshold)
            {
                 continue;
            }
            g_pp.real_rssi = dbm;
            strncpy(g_pp.device_id,g_pdev.device_id,MAC_ADDR_LEN);
            g_pp.type = 1;
            g_pp.ap_or_client = 'c';
            strncpy(g_pp.mac,data,6);
            g_pp.channel = atoi(g_pdev.channel);
            strcpy(g_pp.ssid,g_pdev.ssid);
            g_pp.resvert = 84;
            sendto(g_socket,(char *)&g_pp,sizeof(struct probe_pack),0,(struct sockaddr *)&g_addr,sizeof(struct sockaddr));
            sendtotal++;
            //D("mac:%02X:%02X:%02X:%02X:%02X:%02X,%d",data[0],data[1],data[2],data[3],data[4],data[5],g_pp.real_rssi);
        }
    }
done:
    uloop_timeout_set(timeout,g_interval * 1000); 
    D("get client %d,send %d",total,sendtotal);
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
    uloop_init();
    g_send_data_timer.cb=probe_data_sender;
    uloop_timeout_set(&g_send_data_timer,g_interval * 1000); 
    uloop_run();
    uloop_done();

done:
    config_exit();
    return 0;

error:
    config_exit();
    return -1;
}
