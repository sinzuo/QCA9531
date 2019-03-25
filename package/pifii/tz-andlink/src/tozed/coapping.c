/*coap ping */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>

#include <coap/coap.h>
#include <coap/coap_dtls.h>

time_t g_timeout = 3;

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

void usage(const char *program){
    const char *p=program;
    p = strrchr( program, '/' );
    if ( p ){
        program = ++p;
    }
    fprintf(stderr,"Usage: %s [-t timeout] <url>\n" 
        "examples:\n"
        "\t%s coap://192.168.1.1\n",program,program);
}

#define hexchar_to_dec(c) ((c) & 0x40 ? ((c) & 0x0F) + 9 : ((c) & 0x0F))

static void
decode_segment(const unsigned char *seg, size_t length, unsigned char *buf) {
  while (length--) {
    if (*seg == '%') {
      *buf = (hexchar_to_dec(seg[1]) << 4) + hexchar_to_dec(seg[2]);
      seg += 2; length -= 2;
    } else {
      *buf = *seg;
    }
    ++buf; 
    ++seg;
  }
}

static int
check_segment(const unsigned char *s, size_t length) {

  size_t n = 0;

  while (length) {
    if (*s == '%') {
      if (length < 2 || !(isxdigit(s[1]) && isxdigit(s[2])))
        return -1;

      s += 2;
      length -= 2;
    }

    ++s; ++n; --length;
  }

  return n;
}

static int
cmdline_input(char *text, str *buf) {
  int len;
  len = check_segment((unsigned char *)text, strlen(text));

  if (len < 0)
    return 0;

  buf->s = (unsigned char *)coap_malloc(len);
  if (!buf->s)
    return 0;

  buf->length = len;
  decode_segment((unsigned char *)text, strlen(text), buf->s);
  return 1;
}

static int ping_send(const char *addr,const int port,char *data,uint32_t datalen)
{
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("socket error\n");
		return -1;
	}   
        struct timeval timeout = {g_timeout,0};
        //wait for 3s
        if( setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout) ) < 0 )
        {    
                close(sock);
                printf("can't set receive timeout!\n");
                return -1;
        }    
     
        //wait for 3s
        if( setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout) ) < 0 )
        {    
                close(sock);
                printf("can't set send timeout!\n");
                return -1;
        }
        	
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family=AF_INET;
	//addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);
        /*
        struct ifreq ifr;  
        strcpy(ifr.ifr_name, "wan");
        if (ioctl(sock,SIOCGIFBRDADDR, &ifr) <  0){ 
                perror("ioctl get broadaddr");  
                return -1;
        }
        //printf("%s\n", inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr)); 
        addrto.sin_addr = ((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr; 
        */
	inet_pton(AF_INET, addr, &addrto.sin_addr);
	addrto.sin_port=htons(port);
	int nlen=sizeof(addrto);

	int ret=sendto(sock, data, datalen, 0, (struct sockaddr*)&addrto, nlen);
	if(0 < ret){
            char recvbuf[64] = {0};
            int recvlen = 0;
            ret = recvfrom(sock,recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&addrto,&recvlen);
            if(0 < recvlen){
                printf("reply from %s\n",inet_ntoa(((struct sockaddr_in *)&addrto)->sin_addr));
            }else{
                printf("coapping no reply\n");
            }
	}else{
	    printf("coapping send failed\n");
        }
        close(sock);
	return 0;
}
/*
coap_pdu_t *ping_pdu(){
    uint16_t tid=0;
    prng((uint8_t*)&tid,sizeof(tid));
    coap_pdu_t * pdu=coap_pdu_init(COAP_MESSAGE_CON,0,tid,60);
    return pdu;
}
*/
int main(int argc,char *argv[]){
    str payload={0,NULL};
    coap_uri_t uri;
    int opt;
    uint16_t tid=0;
    prng((uint8_t*)&tid,sizeof(tid));

    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
        case 't':
            g_timeout = atoi(optarg);
            break;
        default:
            usage(argv[0]);
            exit(1);
        }
    }
    if (optind < argc) {
        if (coap_split_uri((unsigned char *)argv[optind], strlen(argv[optind]), &uri) < 0) {
          coap_log(LOG_ERR, "invalid CoAP URI\n");
          exit(1);
        }
    } else {
        usage(argv[0]);
        exit(1);
    }

    coap_pdu_t *pdu = coap_pdu_init(COAP_MESSAGE_CON,0,tid,60);
    coap_pdu_encode_header(pdu,COAP_PROTO_UDP);
    char addr[128]={0};
    memcpy(addr,uri.host.s,uri.host.length);
    ping_send(addr,uri.port, pdu->token - pdu->hdr_size,
				  pdu->used_size + pdu->hdr_size);
    coap_delete_pdu(pdu);
    return 0;
}

