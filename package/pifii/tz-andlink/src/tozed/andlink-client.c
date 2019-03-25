
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <coap/coap.h>
#include <coap/coap_dtls.h>
#include "coap_list.h"


#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__ ((unused))
#else /* not a GCC */
#define UNUSED_PARAM
#endif /* GCC */

static int g_ready = 0;
static str g_payload = { 0, NULL };
static str g_token = { 0, NULL };
static uint32_t g_recvmaxlen = 1024;
static uint32_t g_recvlen = 0;
static unsigned char *g_recvbufpoint = NULL;
static coap_list_t *g_send_optlist = NULL;
static coap_block_t g_block = { .num = 0, .m = 0, .szx = 5 };
static unsigned int g_wait_seconds = 90;
static unsigned int g_wait_ms = 0;
static int g_wait_ms_reset = 0;

static int
order_opts(void *a, void *b) {
  coap_option *o1, *o2;

  if (!a || !b)
    return a < b ? -1 : 1;

  o1 = (coap_option *)(((coap_list_t *)a)->data);
  o2 = (coap_option *)(((coap_list_t *)b)->data);

  return (COAP_OPTION_KEY(*o1) < COAP_OPTION_KEY(*o2))
    ? -1
    : (COAP_OPTION_KEY(*o1) != COAP_OPTION_KEY(*o2));
}

static inline int
check_token(coap_pdu_t *received) {
  return received->token_length == g_token.length &&
    memcmp(received->token, g_token.s, g_token.length) == 0;
}

static coap_list_t *
new_option_node(uint16_t key, size_t length, unsigned char *data) {
  coap_list_t *node;

  node = coap_malloc(sizeof(coap_list_t) + sizeof(coap_option) + length);

  if (node) {
    coap_option *option;
    option = (coap_option *)(node->data);
    COAP_OPTION_KEY(*option) = key;
    COAP_OPTION_LENGTH(*option) = length;
    memcpy(COAP_OPTION_DATA(*option), data, length);
  } else {
    coap_log(LOG_DEBUG, "new_option_node: malloc\n");
  }

  return node;
}

static coap_pdu_t *
coap_new_request_pdu(coap_context_t *ctx,
                 coap_session_t *session,
                 unsigned char m,
                 coap_list_t **options,
                 unsigned char *data,
                 size_t length){
  //new request ,pdu
  coap_pdu_t *pdu;
  coap_list_t *opt;
  (void)ctx;
  
  if (!(pdu = coap_new_pdu(session))){
      debug("cannot add token to request\n");
      return NULL;
  }

  pdu->type = COAP_MESSAGE_CON;
  pdu->tid = coap_new_message_id(session);
  pdu->code = m;

  pdu->token_length = g_token.length;
  if ( !coap_add_token(pdu, g_token.length, g_token.s)) {
    debug("cannot add token to request\n");
  }

  if (options) {
    /* sort options for delta encoding */
    LL_SORT((*options), order_opts);

    LL_FOREACH((*options), opt) {
      coap_option *o = (coap_option *)(opt->data);
      coap_add_option(pdu,
                      COAP_OPTION_KEY(*o),
                      COAP_OPTION_LENGTH(*o),
                      COAP_OPTION_DATA(*o));
    }
  }

  if (length > 0 ) {
    if (length > (1u << (g_block.szx + 4)))
      coap_add_block(pdu, length, data, g_block.num, g_block.szx);
    else
      coap_add_data(pdu, length, data);
  }
  return pdu;
}
static void
response_process_hnd(struct coap_context_t *ctx,
                coap_session_t *session,
                coap_pdu_t *sent,
                coap_pdu_t *received,
                const coap_tid_t id UNUSED_PARAM){

  coap_pdu_t *pdu = NULL;
  coap_opt_t *block_opt;
  coap_opt_iterator_t opt_iter;
  unsigned char buf[4];
  coap_list_t *option;
  size_t len;
  unsigned char *databuf;
  coap_tid_t tid;

  coap_show_pdu(received);

  /* check if this is a response to our original request */
  if (!check_token(received)) {
    /* drop if this was just some message, or send RST in case of notification */
    if (!sent && (received->type == COAP_MESSAGE_CON ||
                  received->type == COAP_MESSAGE_NON))
      coap_send_rst(session, received);
    return;
  }

  if (received->type == COAP_MESSAGE_RST) {
    info("got RST\n");
    return;
  }

  /* output the received data, if any */
  if (COAP_RESPONSE_CLASS(received->code) == 2) {

    /* Got some data, check if block option is set. Behavior is undefined if
     * both, Block1 and Block2 are present. */
    block_opt = coap_check_option(received, COAP_OPTION_BLOCK2, &opt_iter);
    if (block_opt) { /* handle Block2 */
      uint16_t blktype = opt_iter.type;

      /* TODO: check if we are looking at the correct block number */
      if (coap_get_data(received, &len, &databuf) && NULL != g_recvbufpoint){
        //append_to_output(databuf, len);
        if(g_recvlen + len < g_recvmaxlen){
            memcpy(g_recvbufpoint + g_recvlen,databuf,len);
            g_recvlen = g_recvlen + len;
        }else{
            memcpy(g_recvbufpoint + g_recvlen,databuf,g_recvmaxlen - g_recvlen);
            g_recvlen = g_recvmaxlen;
            g_ready = 1;
            return ;
        }
      }
      if(COAP_OPT_BLOCK_MORE(block_opt)) {
        /* more bit is set */
        debug("found the M bit, block size is %u, block nr. %u\n",
              COAP_OPT_BLOCK_SZX(block_opt),
              coap_opt_block_num(block_opt));

        /* create pdu with request for next block */
        pdu = coap_new_request_pdu(ctx, session, COAP_REQUEST_POST, NULL, NULL, 0); /* first, create bare PDU w/o any option  */
        if ( pdu ) {
          /* add URI components from optlist */
          for (option = g_send_optlist; option; option = option->next ) {
            coap_option *o = (coap_option *)(option->data);
            switch (COAP_OPTION_KEY(*o)) {
              case COAP_OPTION_URI_HOST :
              case COAP_OPTION_URI_PORT :
              case COAP_OPTION_URI_PATH :
              case COAP_OPTION_URI_QUERY :
                coap_add_option (pdu,
                                 COAP_OPTION_KEY(*o),
                                 COAP_OPTION_LENGTH(*o),
                                 COAP_OPTION_DATA(*o));
                break;
              default:
                ;     /* skip other options */
            }
          }

          /* finally add updated block option from response, clear M bit */
          /* blocknr = (blocknr & 0xfffffff7) + 0x10; */
          debug("query block %d\n", (coap_opt_block_num(block_opt) + 1));
          coap_add_option(pdu,
                          blktype,
                          coap_encode_var_bytes(buf,
                                 ((coap_opt_block_num(block_opt) + 1) << 4) |
                                  COAP_OPT_BLOCK_SZX(block_opt)), buf);

          tid = coap_send(session, pdu);

          if (tid == COAP_INVALID_TID) {
            debug("message_handler: error sending new request");
          } else {
	    g_wait_ms = g_wait_seconds * 1000;
	    g_wait_ms_reset = 1;
          }

          return;
        }
      }
    } else { /* no Block2 option */
      block_opt = coap_check_option(received, COAP_OPTION_BLOCK1, &opt_iter);

      if (block_opt) { /* handle Block1 */
        unsigned int szx = COAP_OPT_BLOCK_SZX(block_opt);
        unsigned int num = coap_opt_block_num(block_opt);
        debug("found Block1 option, block size is %u, block nr. %u\n", szx, num);
        if (szx != g_block.szx) {
          unsigned int bytes_sent = ((g_block.num + 1) << (g_block.szx + 4));
          if (bytes_sent % (1 << (szx + 4)) == 0) {
            /* Recompute the block number of the previous packet given the new block size */
            g_block.num = (bytes_sent >> (szx + 4)) - 1;
            g_block.szx = szx;
            debug("new Block1 size is %u, block number %u completed\n", (1 << (g_block.szx + 4)), g_block.num);
          } else {
            debug("ignoring request to increase Block1 size, "
            "next block is not aligned on requested block size boundary. "
            "(%u x %u mod %u = %u != 0)\n",
                  g_block.num + 1, (1 << (g_block.szx + 4)), (1 << (szx + 4)),
                  bytes_sent % (1 << (szx + 4)));
          }
        }

        if (g_payload.length <= (g_block.num+1) * (1 << (g_block.szx + 4))) {
          debug("upload ready\n");
          g_ready = 1;
          return;
        }

        /* create pdu with request for next block */
        pdu = coap_new_request_pdu(ctx, session, COAP_REQUEST_POST, NULL, NULL, 0); /* first, create bare PDU w/o any option  */
        if (pdu) {

          /* add URI components from optlist */
          for (option = g_send_optlist; option; option = option->next ) {
            coap_option *o = (coap_option *)(option->data);
            switch (COAP_OPTION_KEY(*o)) {
              case COAP_OPTION_URI_HOST :
              case COAP_OPTION_URI_PORT :
              case COAP_OPTION_URI_PATH :
              case COAP_OPTION_CONTENT_FORMAT :
              case COAP_OPTION_URI_QUERY :
                coap_add_option (pdu,
                                 COAP_OPTION_KEY(*o),
                                 COAP_OPTION_LENGTH(*o),
                                 COAP_OPTION_DATA(*o));
                break;
              default:
              ;     /* skip other options */
            }
          }

          /* finally add updated block option from response, clear M bit */
          /* blocknr = (blocknr & 0xfffffff7) + 0x10; */
          g_block.num++;
          g_block.m = ((g_block.num+1) * (1 << (g_block.szx + 4)) < g_payload.length);

          debug("send block %d\n", g_block.num);
          coap_add_option(pdu,
                          COAP_OPTION_BLOCK1,
                          coap_encode_var_bytes(buf,
                          (g_block.num << 4) | (g_block.m << 3) | g_block.szx), buf);

          coap_add_block(pdu,
                         g_payload.length,
                         g_payload.s,
                         g_block.num,
                         g_block.szx);
          coap_show_pdu(pdu);

	  tid = coap_send(session, pdu);

          if (tid == COAP_INVALID_TID) {
            debug("message_handler: error sending new request");
          } else {
	    g_wait_ms = g_wait_seconds * 1000;
	    g_wait_ms_reset = 1;
          }

          return;
        }
      } else {
        /* There is no block option set, just read the data and we are done. */
        if (coap_get_data(received, &len, &databuf) && NULL != g_recvbufpoint){
            //append_to_output(databuf, len);
            g_recvlen = min(g_recvmaxlen, len);
            memcpy(g_recvbufpoint, databuf, g_recvlen);
        }
      }
    }
  } else {      /* no 2.05 */

    /* check if an error was signaled and output payload if so */
    if (COAP_RESPONSE_CLASS(received->code) >= 4) {
      fprintf(stderr, "%d.%02d",
              (received->code >> 5), received->code & 0x1F);
      if (coap_get_data(received, &len, &databuf)) {
        fprintf(stderr, " ");
        while(len--)
        fprintf(stderr, "%c", *databuf++);
      }
      fprintf(stderr, "\n");
    }

  }

  /* any pdu that has been created in this function must be sent by now */
  assert(pdu == NULL);

  /* our job is done, we can exit at any time */
  //ready = coap_check_option(received, COAP_OPTION_OBSERVE, &opt_iter) == NULL;
  g_ready = 1;
}

static int
resolve_address(const str *server, struct sockaddr *dst) {

  struct addrinfo *res, *ainfo;
  struct addrinfo hints;
  static char addrstr[256];
  int error, len=-1;

  memset(addrstr, 0, sizeof(addrstr));
  if (server->length)
    memcpy(addrstr, server->s, server->length);
  else
    memcpy(addrstr, "localhost", 9);

  memset ((char *)&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_UNSPEC;

  error = getaddrinfo(addrstr, NULL, &hints, &res);

  if (error != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
    return error;
  }

  for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
    switch (ainfo->ai_family) {
    case AF_INET6:
    case AF_INET:
      len = ainfo->ai_addrlen;
      memcpy(dst, ainfo->ai_addr, len);
      goto finish;
    default:
      ;
    }
  }

 finish:
  freeaddrinfo(res);
  return len;
}

#if 1
int send_data_to_gateway(const unsigned char *url,
                         unsigned char *token,
                         unsigned char *senddata,
                         unsigned char *recvbuf,
                         uint32_t  recvmaxlen,
                         uint32_t *recvlen){
  int result = 0;
  coap_context_t  *ctx = NULL;
  coap_session_t *session = NULL;
  coap_pdu_t *pdu = NULL;
  coap_address_t dst;
  coap_uri_t dst_uri;
  void *addrptr = NULL;
  char addr[INET6_ADDRSTRLEN];

  //init var
  g_send_optlist = NULL;
  g_ready = 0;
  COAP_SET_STR(&g_payload,strlen(senddata),senddata);
  COAP_SET_STR(&g_token,strlen(token),token);
  g_recvmaxlen = (0 == recvmaxlen ? 1024 : recvmaxlen);
  g_recvlen = 0;
  g_recvbufpoint = recvbuf;
  g_send_optlist = NULL;
  g_block.num = 0; 
  g_block.m = 0;
  g_block.szx = 5 ;    //default payload 512Byte(1 << ( g_block.szx + 4))
  g_wait_seconds = 90; //default 90s
  g_wait_ms = 0;
  g_wait_ms_reset = 0;
  //init var end
 
  if (0 > coap_split_uri(url,strlen(url),&dst_uri)){
      return -1;
  }
  if (dst_uri.path.length) {
      uint8_t _buf[64]={0};
      uint8_t *buf = _buf;
      size_t buflen = 64;
      int res = coap_split_path(dst_uri.path.s, dst_uri.path.length, buf, &buflen);

      while (res--) {
        coap_insert(&g_send_optlist,
                    new_option_node(COAP_OPTION_URI_PATH,
                    coap_opt_length(buf),
                    coap_opt_value(buf)));

        buf += coap_opt_size(buf);
      }
  }
  int addr_size = resolve_address(&dst_uri.host, &dst.addr.sa);
  if (0 >= addr_size){
      result = -1;
      goto finish;
  }
  dst.size = addr_size;
  dst.addr.sin.sin_port = htons( dst_uri.port);
  ctx = coap_new_context( NULL );
  if ( !ctx ) {
    coap_log( LOG_EMERG, "cannot create context\n" );
    result = -1;
    goto finish;
  }
  session = coap_new_client_session( ctx, NULL, &dst, COAP_PROTO_UDP );
  if ( !session ) {
    coap_log( LOG_EMERG, "cannot create client session\n" );
    result = -1;
    goto finish;
  }
  
  coap_register_option(ctx, COAP_OPTION_BLOCK2);
  coap_register_response_handler(ctx, response_process_hnd);

  /* add Uri-Host if server address differs from uri.host */
  switch (dst.addr.sa.sa_family) {
  case AF_INET:
    addrptr = &dst.addr.sin.sin_addr;
    /* create context for IPv4 */
    break;
  case AF_INET6:
    addrptr = &dst.addr.sin6.sin6_addr;
    break;
  default:
    ;
  }
  if (addrptr 
      && (inet_ntop(dst.addr.sa.sa_family, addrptr, addr, sizeof(addr)) != 0)
      && (strlen(addr) != dst_uri.host.length
      || memcmp(addr, dst_uri.host.s, dst_uri.host.length) != 0)) {
        /* add Uri-Host */
        coap_insert(&g_send_optlist,
                    new_option_node(COAP_OPTION_URI_HOST,
                    dst_uri.host.length,
                    dst_uri.host.s));
  }
  
  if (g_payload.length > 0) {
    if (g_payload.length > (1u << (g_block.szx + 4))){
        unsigned char buf[2]={0};
        g_block.m = 1;
        coap_insert(&g_send_optlist, new_option_node(COAP_OPTION_BLOCK1, 
                coap_encode_var_bytes(buf,(g_block.num << 4 | g_block.m << 3 | g_block.szx)),
                buf));
    }
  }

  pdu = coap_new_request_pdu(ctx,session,
                             COAP_REQUEST_POST,
                             &g_send_optlist,
                             g_payload.s, g_payload.length);
  
  result = coap_send(session, pdu);

  if (COAP_INVALID_TID == result) {
    coap_log( LOG_EMERG, "cannot create client session\n" );
    goto finish;
  }
  //run 
  g_wait_ms = g_wait_seconds * 1000;
  g_ready = 0;
  debug("timeout is set to %u seconds\n", g_wait_seconds);

  while ( !(g_ready && coap_can_exit(ctx)) ) {
    result = coap_run_once( ctx, min(g_wait_ms, 1000));
    if ( result >= 0 ) {
      if ( g_wait_ms > 0 && !g_wait_ms_reset ) {
    	if ( (unsigned)result >= g_wait_ms ) {
    	  info( "timeout\n" );
    	  break;
    	} else {
    	  g_wait_ms -= result;
    	}
      }
      g_wait_ms_reset = 0;
    }
  }

  *recvlen = g_recvlen;
  result = 0;

 finish:

  coap_delete_list(g_send_optlist);
  coap_session_release( session );
  coap_free_context( ctx );
  coap_cleanup();
  //close_output();
  return result;
}

int main(int argc,char *argv[])
{
    uint8_t buf[512]={0};
    uint32_t len=0;
    if(3 == argc){
        send_data_to_gateway(argv[1],"1234",argv[2],buf,sizeof(buf),&len);
        printf("recv data:%s\n",buf);
    }
}
#endif

