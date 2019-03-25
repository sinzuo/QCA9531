/*coap for broadcast client */

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

uint8_t g_coap_message = COAP_MESSAGE_CON; //default NON

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

void usage(const char *program){
    const char *p=program;
    p = strrchr( program, '/' );
    if ( p ){
        program = ++p;
    }
    fprintf(stderr,"Usage: %s [-e text]\n" 
        "\t-e text\t\tinclude text as payload (use percent-encoding for\n"
        "\t\t\tnon-ASCII characters)\n"
        "examples:\n"
        "\t%s coap://192.168.3.255/qlink/searchgw -e Hello\n"
        "\t%s coap://255.255.255.255/qlink/searchgw -e Hello\n",program,program,program);
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

static int broadcast_send(const char *addr,const int port,char *data,uint32_t datalen)
{
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("socket error\n");
		return -1;
	}   
	
	const int opt = 1;
	int nb = 0;
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1){
		printf("set socket error...\n");
		return -1;
	}
        struct timeval timeout = {3,0};
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
	if(ret <= 0){
		printf("send failed\n");
	}
	else if(COAP_MESSAGE_CON == g_coap_message){
	        struct sockaddr_in addrfrom;
	        bzero(&addrfrom, sizeof(struct sockaddr_in));
                uint8_t datafrom[128]={0};
	        ret=recvfrom(sock, datafrom, 128, 0, (struct sockaddr*)&addrfrom, &nlen);
                coap_pdu_t *pdu = coap_pdu_init(0, 0, 0,128);
                if(ret > 0 && coap_pdu_parse(COAP_PROTO_UDP,datafrom,128,pdu)){
                    uint8_t *pdudata = NULL;
                    size_t pdudata_len = 0;
                    coap_get_data(pdu,&pdudata_len,&pdudata);
                    if(pdudata && 0 < pdudata_len){
		        //printf("%s\n",pdudata);
                        if(strstr(pdudata,"\"searchAck\":\"ANDLINK-GW\"")){
                            uint8_t ip[24] = {0}; 
                            inet_ntop(AF_INET, &addrfrom.sin_addr, ip, sizeof(ip));
		            printf("%s\n",ip);
                        }
                    }
                }
                coap_delete_pdu(pdu);
	}
	return 0;
}

coap_pdu_t *broadcast_pdu(str *uri_path,str *data){
    uint16_t tid=0;
    prng((uint8_t*)&tid,sizeof(tid));
    //coap_pdu_t * pdu=coap_pdu_init(COAP_MESSAGE_NON,COAP_REQUEST_POST,tid,4096);
    coap_pdu_t * pdu=coap_pdu_init(g_coap_message,COAP_REQUEST_POST,tid,512);
    uint8_t _buf[128]={0};
    uint8_t *buf = _buf;
    size_t buflen = 128;

    if(uri_path && 0 < uri_path->length){
        uint32_t res = coap_split_path(uri_path->s,uri_path->length, buf, &buflen);
        while (res--) {
            coap_add_option(pdu,COAP_OPTION_URI_PATH,
                            coap_opt_length(buf),
                            coap_opt_value(buf));

            buf += coap_opt_size(buf);
        }
    }
    if(data && 0 < data->length){
        coap_add_data(pdu,data->length,data->s);
    }
    return pdu;
}

int main(int argc,char *argv[]){
    str payload={0,NULL};
    coap_uri_t uri;
    int opt;
    while ((opt = getopt(argc, argv, "Ne:")) != -1) {
        switch (opt) {
        case 'e':
            if (!cmdline_input(optarg, &payload));
            break;
        case 'N':
            g_coap_message = COAP_MESSAGE_NON;
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

    coap_pdu_t *pdu = broadcast_pdu(&uri.path,&payload);
    coap_pdu_encode_header(pdu,COAP_PROTO_UDP);
    char addr[128]={0};
    memcpy(addr,uri.host.s,uri.host.length);
    broadcast_send(addr,uri.port, pdu->token - pdu->hdr_size,
					pdu->used_size + pdu->hdr_size);
    coap_delete_pdu(pdu);
    return 0;
}

