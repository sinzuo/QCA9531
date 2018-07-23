#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>   
#include <sys/socket.h> 
#include <net/if.h>  
#include <net/if_arp.h>  
#include <sys/ioctl.h> 
#include <sys/socket.h>  
#include <netpacket/packet.h>  
#include <stdlib.h>
#include <arpa/inet.h>

#define LEN 64
int main()
{
    int    result=0, fd, recv_count=5; 
    struct sockaddr_ll   sa;   
    struct ifreq    ifr;  
    struct timeval timeout = {10,0};
    char const *network_dev_name="br-lan";

    char  buf[LEN]={0};  
    char  mac_addr[6]={0xff,0xff,0xff,0xff,0xff,0xff};
    unsigned short type = htons(0x2696);//9878

    memcpy(buf, mac_addr, 6);
    memset(&sa, 0, sizeof(sa));  
    sa.sll_family = PF_PACKET;  
    sa.sll_protocol = type;  
    //create socket  
    fd = socket(PF_PACKET, SOCK_RAW, type);  
    if(fd < 0){  
        printf("socket error\n");  
        return -1;  
    }  
    //wait for 3s
    if( setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout) ) < 0 )
    {
        printf("can't set receive timeout!");
        return -1;
    }
    
    //wait for 3s
    if( setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout) ) < 0 )
    {
        printf("can't set send timeout!");
        return -1;
    }
    //printf("ifname:%s\n",network_dev_name);
    // get flags   
    strcpy(ifr.ifr_name,network_dev_name);  
    result = ioctl(fd, SIOCGIFFLAGS, &ifr);  
    if(result != 0){   
        printf("ioctl error, get flags\n");  
        return -1;  
    }         
   //need mac addr
    if( mac_addr ){
        //copy network card's name
        strncpy(ifr.ifr_name, network_dev_name, sizeof(ifr.ifr_name));
        //get mac address
        if( ioctl(fd, SIOCGIFHWADDR, &ifr) < 0 )
        {
        	printf("can't send broadcast packets!");
        	return -1;
        }
        
        memcpy(mac_addr, ifr.ifr_hwaddr.sa_data,6);
        memcpy(buf+6,mac_addr,6);
        memcpy(buf+12,&type,2);
    }
    /*
    ifr.ifr_flags |= IFF_PROMISC;  
    // set promisc mode  
    result = ioctl(fd, SIOCSIFFLAGS, &ifr);  
    if(result != 0){  
            perror("ioctl error, set promisc\n");  
            return errno;  
    }  
    */ 

    //get index  
    result = ioctl(fd, SIOCGIFINDEX, &ifr);  
    if(result != 0){  
        printf("ioctl error, get index\n");  
        return -1;  
    }  
    sa.sll_ifindex = ifr.ifr_ifindex;  

    //bind fd  
    result = bind(fd, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll));  
    if(result != 0){  
        printf("bind error\n");  
        return -1;  
    }
    int i = 0;
    for(;i<14;i++)
    {
        printf("%02X",(unsigned char)buf[i]);
    }
    printf("\n");
    result = send(fd,buf,14,0);
    if(0 >= result){
        perror("send error:");
    } 
    printf("sendlen=%d\n",result); 
    /*
    while(0 < recv_count){
        result = recv(fd,buf,LEN,0);
        if(0 < result){
            break; 
        }
        recv_count--;
    }
    printf("revc_len=%d\n",result);
    */
    close(fd);
    return 0; 
}
