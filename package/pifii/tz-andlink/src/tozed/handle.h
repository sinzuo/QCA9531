#ifndef __HANDLE_H__
#define __HANDLE_H__

#include<coap/resource.h>

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__ ((unused))
#else /* not a GCC */
#define UNUSED_PARAM
#endif /* GCC */

extern unsigned char *g_scriptpoint;

#define HANDLE_ARGS coap_context_t *ctx UNUSED_PARAM,\
              struct coap_resource_t *resource UNUSED_PARAM,\
              coap_session_t *session UNUSED_PARAM,\
              coap_pdu_t *request UNUSED_PARAM,\
              str *token UNUSED_PARAM,\
              str *query UNUSED_PARAM,\
              coap_pdu_t *response

typedef void (*process_handler_t)(
            uint8_t *,
            size_t ,
            uint8_t *,
            size_t );
/*
typedef struct andlink_resources_handle_t{
    str uri_path;
    unsigned char method;
    coap_method_handler_t hnd;
} andlink_resources_handle_t;

extern andlink_resources_handle_t andlink_hdl_list[];
*/
typedef struct resources_handle_t{
    uint8_t uri_path[64];
    unsigned char method;
    coap_method_handler_t hnd;
    struct resources_handle_t *next;
} resources_handle_t;

typedef struct resources_handle_list{
    resources_handle_t *head;
    resources_handle_t *tail;    
}resources_handle_list;


resources_handle_list *
add_resources_handle_list(resources_handle_list *prhl,
                                        resources_handle_t *prh);

void free_resources_handle_list(resources_handle_list *prhl);

int resources_handle_read(resources_handle_list *prhl);

static void resources_process_handle(coap_context_t *ctx UNUSED_PARAM,
              struct coap_resource_t *resource UNUSED_PARAM,
              coap_session_t *session UNUSED_PARAM,
              coap_pdu_t *request UNUSED_PARAM,
              str *token UNUSED_PARAM,
              str *query UNUSED_PARAM,
              coap_pdu_t *response);

//static void qlink_request(HANDLE_ARGS);

//static void qlink_success(HANDLE_ARGS);
#endif
