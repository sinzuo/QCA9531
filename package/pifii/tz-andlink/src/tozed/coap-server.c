/*coap server */

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

#include "handle.h"

#define COAP_RESOURCE_CHECK_TIME 2

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

static char *g_loglevels[] = {
  "EMRG", "ALRT", "CRIT", "ERR", "WARN", "NOTE", "INFO", "DEBG" 
};

/* temporary storage for dynamic resource representations */
static int quit = 0;

static resources_handle_list g_rhl={NULL,NULL};

void usage(const char *program){
    const char *p=program;
    p = strrchr( program, '/' );
    if ( p ){
        program = ++p;
    }
    fprintf(stderr,"Usage: %s [-v num]\n"
            "\t-g \t\tforeground\n"
            "\t-v num \t\tverbosity level (default: 3)\n"
            "\t-s file\t\tresources process script or program\n",program);
}

void log_print(coap_log_t level, const char *message){
    FILE *log_fd = fopen("/tmp/tzcoapserver.log","a+");
    if(!log_fd){
        return ;
    }
    char stamp[32]={0};
    time_t timep;
    struct tm *ptm;
    time(&timep);
    ptm = localtime(&timep);
    strftime(stamp,sizeof(stamp),"%Y-%m-%d %H:%M:%S",ptm);
    fprintf(log_fd, "%s[%s] %s",stamp,g_loglevels[level],message);
    
    fflush(log_fd);
    fclose(log_fd);
}
void log_null(coap_log_t level, const char *message){
}

/* SIGINT handler: set quit to 1 for graceful termination */
static void
handle_sigint(int signum UNUSED_PARAM) {
  quit = 1;
}

static void 
init_resources(coap_context_t *ctx) {
    coap_resource_t *r;
    resources_handle_t *p = NULL; 
    for(p = g_rhl.head; p && p->hnd ;p = p->next){
        r = coap_resource_init(p->uri_path, strlen(p->uri_path), 0);
        coap_register_handler(r, p->method, p->hnd);

        coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char *)"0", 1, 0);
        coap_add_attr(r, (unsigned char *)"title", 5, (unsigned char *)"\"AndLink\"", 9, 0);
        coap_add_resource(ctx, r);
    }
}

static void
fill_keystore(coap_context_t *ctx) {
  static uint8_t key[] = "secretPSK";
  size_t key_len = sizeof( key ) - 1;
  coap_context_set_psk( ctx, "CoAP", key, key_len );
}

static coap_context_t *
get_context(const char *node, const char *port) {
  coap_context_t *ctx = NULL;
  int s;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  ctx = coap_new_context(NULL);
  if (!ctx) {
    return NULL;
  }
  /* Need PSK set up before we set up (D)TLS endpoints */
  fill_keystore(ctx);

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Coap uses UDP */
  hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

  s = getaddrinfo(node, port, &hints, &result);
  if ( s != 0 ) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    coap_free_context(ctx);
    return NULL;
  }

  /* iterate through results until success */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    coap_address_t addr, addrs;
    coap_endpoint_t *ep_udp = NULL, *ep_dtls = NULL, *ep_tcp = NULL, *ep_tls = NULL;

    if (rp->ai_addrlen <= sizeof(addr.addr)) {
      coap_address_init(&addr);
      addr.size = rp->ai_addrlen;
      memcpy(&addr.addr, rp->ai_addr, rp->ai_addrlen);

      addrs = addr;
      if (addr.addr.sa.sa_family == AF_INET) {
        addrs.addr.sin.sin_port = htons(ntohs(addr.addr.sin.sin_port) + 1);
      } else if (addr.addr.sa.sa_family == AF_INET6) {
        addrs.addr.sin6.sin6_port = htons(ntohs(addr.addr.sin6.sin6_port) + 1);
      } else {
        goto finish;
      }

      ep_udp = coap_new_endpoint(ctx, &addr, COAP_PROTO_UDP);
      if (ep_udp) {
          if (coap_dtls_is_supported()) {
        	  ep_dtls = coap_new_endpoint(ctx, &addrs, COAP_PROTO_DTLS);
        	  if (!ep_dtls)
        	      coap_log(LOG_CRIT, "cannot create DTLS endpoint\n");
          }
      } else {
          coap_log(LOG_CRIT, "cannot create UDP endpoint\n");
          continue;
      }
      /*
      ep_tcp = coap_new_endpoint(ctx, &addr, COAP_PROTO_TCP);
      if (ep_tcp) {
          if (coap_tls_is_supported()) {
    	      ep_tls = coap_new_endpoint(ctx, &addrs, COAP_PROTO_TLS);
    	      if (!ep_tls)
    	          coap_log(LOG_CRIT, "cannot create TLS endpoint\n");
    	}
      } else {
          coap_log(LOG_CRIT, "cannot create TCP endpoint\n");
      }
      */
      if (ep_udp)
	    goto finish;
    }
  }

  fprintf(stderr, "no context available for interface '%s'\n", node);

finish:
  freeaddrinfo(result);
  return ctx;
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
        //fclose(stdin);
        //fclose(stdout);
        fclose(stderr);

}

int
main(int argc, char **argv) {
    coap_context_t  *ctx;
    //char addr_str[NI_MAXHOST] = "::";
    char port_str[NI_MAXSERV] = "5683";
    coap_log_t log_level = LOG_ERR;
    char foreground=0;
    char islog=0;
    unsigned wait_ms;
    int opt;
    while ((opt = getopt(argc, argv, "glv:s:")) != -1) {
        switch (opt) {
        case 'v' :
            log_level = strtol(optarg, NULL, 10);
            break;
        case 'g' :
            foreground = 1;
            break;
        case 'l' :
            islog = 1;
            break;
        case 's' :
            g_scriptpoint = (unsigned char*)malloc(strlen(optarg)+1); 
            memcpy(g_scriptpoint,optarg,strlen(optarg));
            g_scriptpoint[strlen(optarg)] = 0;
            break;
        default:
            usage(argv[0]);
            exit(1);
        }
    }
    if(1 == islog){
        coap_set_log_handler(log_print);
    }else{
        coap_set_log_handler(log_null);
    }
    coap_set_log_level(log_level);

    //ctx = get_context(addr_str, port_str);
    ctx = get_context(NULL, port_str);  
    if (!ctx)
        goto end;

    if (0 > resources_handle_read(&g_rhl)){
        goto end;
    }

    init_resources(ctx);

    //signal(SIGINT, handle_sigint);
    //if(!foreground)
    //    deamon();
  
    wait_ms = COAP_RESOURCE_CHECK_TIME * 1000;

    while ( !quit ) {
        int result = coap_run_once( ctx, wait_ms );
        if ( result < 0 ) {
            break;
        } else if ( (unsigned)result < wait_ms ) {
            wait_ms -= result;
        } else {
            wait_ms = COAP_RESOURCE_CHECK_TIME * 1000;
        }
    }

end:
    free_resources_handle_list(&g_rhl);
    coap_free_context(ctx);
    coap_cleanup();
    if(g_scriptpoint){
        free(g_scriptpoint);
    }

    return 0;
}
