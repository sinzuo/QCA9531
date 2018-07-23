#include <pcap.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define MAC_ADDR_LEN 6
#define MAC_MAX_STR  17
#define MAC_MIN_STR   12

#define DHCP_TYPE_ACK 5
#define DHCP_TYPE_OFFSET 284
#define DHCP_CLIENT_IP_OFFSET 58
#define DHCP_CLIENT_MAC_OFFSET 70

#define LINE_BUFSIZE 1024

const u_char FFMAC[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const u_char ZOREIP[4]={0x00,0x00,0x00,0x00};

typedef struct dhcp_info
{
    time_t time;
    u_char mac[6];
    u_char ip[4];
    struct dhcp_info *next;
}dhcp_info;

typedef struct dhcp_list
{
    dhcp_info *head;      
    dhcp_info *tail;      
}dhcp_list;

dhcp_list g_dhcp_list={NULL,NULL};

dhcp_list *add_dhcp_info_list(dhcp_list *pdl,dhcp_info *pdi)
{
    if(NULL == pdl || NULL == pdi)
    {
        return pdl;
    }
    if(NULL == pdl->head)
    {
        pdl->head=pdi;
        pdl->tail=pdi;
    }
    else
    {
        pdl->tail->next=pdi;
        pdl->tail=pdl->tail->next;
    }
    return pdl;
}

dhcp_info *find_dhcp_info_list(dhcp_list *pdl,dhcp_info *pdi)
{
    
    if(NULL == pdl || NULL == pdl->head || NULL == pdi)
    {
        return NULL;
    }
    
    dhcp_info *p=pdl->head;
    while(p) 
    {
        if(0 == memcmp(pdi->mac,p->mac,6))
        {
            return p;
        }
        p=p->next;
    }
    return NULL;
}

void print_dhcp_info_list(dhcp_list *pdl)
{
    if(NULL == pdl || NULL == pdl->head)
    {
        return ;
    }
    dhcp_info *p=pdl->head;
    while(p)
    {
        printf("%ld %02X:%02X:%02X:%02X:%02X:%02X %d.%d.%d.%d\n",p->time,
                p->mac[0],p->mac[1],p->mac[2],p->mac[3],p->mac[4],p->mac[5],
                p->ip[0],p->ip[1],p->ip[2],p->ip[3]);
        p=p->next;
    }
}

void free_dhcp_info_list(dhcp_list *pdl)
{
    if(NULL != pdl && NULL != pdl->head)
    {
        dhcp_info *p=pdl->head;
        while(p) 
        {
            pdl->head=pdl->head->next;
            free(p);
            p=pdl->head;
        }
        pdl->head=NULL;
        pdl->tail=NULL;
    }
}

char *tomac(const char *pszStr,char *pmac)
{
    char * pszTmpStr = NULL;    
    if(NULL == pszStr || NULL == pmac) 
    {
        return NULL; 
    }
    if( MAC_MIN_STR != strlen(pszStr) && MAC_MAX_STR != strlen(pszStr))
    {
        //printf("error:mac string size is not 12 or 17\n");
        return NULL; 
    }
    else if(MAC_MAX_STR == strlen(pszStr))
    {
        pszTmpStr = (char*)malloc(MAC_MIN_STR); 
        int i = 0,j = 0;
        while(i < MAC_MIN_STR && j < MAC_MAX_STR)
        {
           if(':' != pszStr[j])
           {
              pszTmpStr[i] =  pszStr[j];
              i++;
           }
           j++;
        }
        
    }
    else 
    {
        pszTmpStr = (char*)malloc(MAC_MIN_STR); 
        strncpy(pszTmpStr,pszStr,MAC_MIN_STR);
    }
    int iLoop = 0; 
    unsigned char *pszTmp = (unsigned char*)malloc(MAC_ADDR_LEN); 
    bzero(pszTmp,6);
    char *pLoop = pszTmpStr;
    char *pLoopTwo = pLoop + 1;
    while(iLoop < MAC_MIN_STR)
    {
        pLoop = pszTmpStr + iLoop;
        pLoopTwo = pLoop + 1;
        if(48 <= *pLoop && 57 >= *pLoop) //0~9 
        {
            pszTmp[iLoop/2] = (*pLoop - 48) << 4; 
        }
        else if(65 <= *pLoop && 70 >= *pLoop) //A~F
        {
            pszTmp[iLoop/2] = (*pLoop - 55) << 4; 
        }
        else if(97 <= *pLoop && 102 >= *pLoop) //a~f
        { 
            pszTmp[iLoop/2] = (*pLoop -87) << 4; 
        }
        else
        {
            //printf("error:\'%c\' is not hex\n",*pLoop);
            goto error;
        }

        if(48 <= *pLoopTwo && 57 >= *pLoopTwo) //0~9
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 48); 
        }
        else if( 65 <= *pLoopTwo && 70 >= *pLoopTwo) //A~F
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 55);  
        }
        else if( 97 <= *pLoopTwo && 102 >= *pLoopTwo) //a~f 
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 87);   
        }
        else
        {
            //printf("error:\'%c\' is not hex\n",*pLoopTwo);
            goto error;

        }
        
        iLoop = iLoop + 2; 
    }
    if( MAC_MIN_STR == iLoop )
    {
        memcpy(pmac,pszTmp,MAC_ADDR_LEN); 
        goto done;
    }
error:
    free(pszTmp);
    free(pszTmpStr);
    //printf("iLoop:%d",iLoop);
    return NULL;
done:
    //printf("iLoop:%d",iLoop);
    free(pszTmp);
    free(pszTmpStr);
    return pmac; 
}

u_char * toip(const u_char *strip,u_char *ip)
{
    if(NULL == strip || NULL == ip)
    {
        return NULL;
    }
    int len=strlen(strip);
    u_char *p=(u_char *)malloc(sizeof(u_char)*(len+1));
    u_char tmp[4]={'\0'};
    int i=0,j=0;
    for(;i<len;i++)
    {
        if('.' != strip[i] && ('0' > strip[i] || '9' < strip[i])) 
        {
            free(p);
            return NULL;
        }
        p[j++] = strip[i];
    }
    if('.' == p[0]) 
    {
        free(p);
        return NULL;
    }
    p[j]='\0';
    len=strlen(p);
    int num;
    int z=0;
    i=0;
    j=0;
    for(;i<len;i++)
    {
        if('.' == p[i])
        {
           p[i] = '\0';
           num = atoi(p+j);
           if(0 > num || 255 <= num || 4 <= z)
           {
               free(p);
               return NULL;
           }
           tmp[z++] = (u_char)num;
           j = i+1; 
        }
    }
    if(j < i)
    {
        num = atoi(p+j);
        if(0 > num || 255 < num || 4 <= z || 3 > z )
        {
            free(p);
            return NULL;
        }
        tmp[z++] = (u_char)num;
        memcpy(ip,tmp,4);
        free(p);
        return ip;
    }
    return NULL;
}

int write_dhcp(dhcp_info *pdicap)
{
    char buf[LINE_BUFSIZE];
    FILE* file;
  
    dhcp_info *pdi=NULL;
    dhcp_info *p=NULL;

    file = fopen("/tmp/ap_mode_dhcp.list","r");
    if(file)
    {
        char time[16]={'\0'};
        char mac[32]={'\0'};
        char ip[32]={'\0'};
        while(fgets(buf, LINE_BUFSIZE, file))
        {
            sscanf(buf,"%s %s %s",time,mac,ip);
            if(NULL == pdi)
            {
                pdi=(dhcp_info*)malloc(sizeof(dhcp_info));
            }
            memset(pdi,'\0',sizeof(dhcp_info));
            pdi->time=atol(time);
            if(0 >= pdi->time || NULL == tomac(mac,pdi->mac) || NULL == toip(ip,pdi->ip))
            {
                free(pdi);
                pdi=NULL;
                continue;
            }
            p=find_dhcp_info_list(&g_dhcp_list,pdi);
            if(p)
            {
                p->time=pdi->time; 
                memcpy(p->mac,pdi->mac,6);
                memcpy(p->ip,pdi->ip,4);
                free(pdi);
            }
            else
            {
                add_dhcp_info_list(&g_dhcp_list,pdi);
            } 
            pdi = NULL;
        }
        fclose(file);
        file=NULL;
    }
    
    p=find_dhcp_info_list(&g_dhcp_list,pdicap);
    if(NULL == p)
    {
        p=(dhcp_info*)malloc(sizeof(dhcp_info));
        memset(p,'\0',sizeof(dhcp_info));
        add_dhcp_info_list(&g_dhcp_list,p);
    }
    if(p) 
    {
        p->time=pdicap->time; 
        memcpy(p->mac,pdicap->mac,6);
        memcpy(p->ip,pdicap->ip,4);
    }
    file = fopen("/tmp/ap_mode_dhcp.list","wt");
    //fseek(file,0L,SEEK_SET);
    p=g_dhcp_list.head;
    while(p)
    {
        fprintf(file,"%ld %02X:%02X:%02X:%02X:%02X:%02X %d.%d.%d.%d\n",p->time,
                p->mac[0],p->mac[1],p->mac[2],p->mac[3],p->mac[4],p->mac[5],
                p->ip[0],p->ip[1],p->ip[2],p->ip[3]);
        p=p->next;
    }
    //print_dhcp_info_list(&g_dhcp_list);
    fclose(file);
    free_dhcp_info_list(&g_dhcp_list);
    return 0;

}

void getPacket(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)
{
  int * id = (int *)arg;
  
  /* 
  printf("id: %d\n", ++(*id));
  printf("Packet length: %d\n", pkthdr->len);
  printf("Number of bytes: %d\n", pkthdr->caplen);
  printf("Recieved time: %s", ctime((const time_t *)&pkthdr->ts.tv_sec)); 
  */
  
  int i;
  u_char *p=NULL; 
  if( DHCP_TYPE_OFFSET < pkthdr->len && DHCP_TYPE_ACK == packet[DHCP_TYPE_OFFSET]); 
  {
      dhcp_info di;
      di.time=pkthdr->ts.tv_sec;

      //printf("DHCP Message Type: %d\n", packet[DHCP_TYPE_OFFSET]);
      p=packet + DHCP_CLIENT_IP_OFFSET; 
      if(0 == memcmp(p,ZOREIP,4))
      {
          return ;
      }
      memcpy(di.ip,p,4);
      //printf("Client IP:%d.%d.%d.%d\n",p[0],p[1],p[2],p[3]);
      p=packet + DHCP_CLIENT_MAC_OFFSET; 
      if(0 == memcmp(p,FFMAC,6))
      {
          return ;
      }
      memcpy(di.mac,p,6);
      //printf("Client MAC:%02X:%02X:%02X:%02X:%02X:%02X\n",p[0],p[1],p[2],p[3],p[4],p[5]);
      write_dhcp(&di);
  }
  /*
  for(i=0; i<pkthdr->len; ++i)
  {
    printf(" %02x", packet[i]);
    if( (i + 1) % 16 == 0 )
    {
      printf("\n");
    }
  }
  
  printf("\n\n");
  */
}
void deamon(void)
{
        pid_t pid, sid;
        pid = fork();
        if (pid < 0)
                exit(-1);
        if (pid > 0)
                exit(0);

        sid = setsid();
        if (sid < 0) {
                fprintf(stderr,"setsid() returned error\n");
                exit(-1);
        }

        char *directory = ".";
        if ((chdir(directory)) < 0) {
            fprintf(stderr,"chdir() returned error\n");
            exit(-1);
        }
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        //close(STDIN_FILENO);
        //close(STDOUT_FILENO);
        //close(STDERR_FILENO);
}

int main(int argc,const char *argv[])
{
  char errBuf[PCAP_ERRBUF_SIZE], * devStr;
  
  // open a device, wait until a packet arrives
  pcap_t * device = pcap_open_live("br-lan", 512, 1, 0, errBuf);
  
  if(!device)
  {
    printf("error: pcap_open_live(): %s\n", errBuf);
    exit(1);
  }
  deamon(); 
  // construct a filter 
  struct bpf_program filter;
  pcap_compile(device, &filter, "udp and dst port 68", 1, 0);
  pcap_setfilter(device, &filter);
  
  // wait loop forever 
  int id = 0;
  pcap_loop(device, -1, getPacket, (u_char*)&id);
  
  pcap_close(device);
  
  //-------------------------------------------- 
  
  return 0;
}
