#include<coap/coap.h>
#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include"handle.h"

#define BLOCK_SZX 5 
#define FREE(x) do { free(x); x = NULL; } while (0);

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

unsigned char *g_scriptpoint = NULL;
static unsigned int g_block_szx = BLOCK_SZX;
static unsigned char g_script[]="/usr/sbin/coap-resources-process";

resources_handle_list *
add_resources_handle_list(resources_handle_list *prhl,
                                        resources_handle_t *prh)
{
    if(NULL == prhl || NULL == prh)
    {   
        return prhl;
    }   
    if(NULL == prhl->head)
    {   
        prhl->head=prh;
        prhl->tail=prh;
    }   
    else
    {   
        prhl->tail->next=prh;
        prhl->tail=prhl->tail->next;
    }   
    return prhl;
}

void free_resources_handle_list(resources_handle_list *prhl)
{
    if(NULL != prhl && NULL != prhl->head)
    {
        resources_handle_t *p=prhl->head;
        while(p)
        {
            prhl->head=prhl->head->next;
            free(p);
            p=prhl->head;
        }
        prhl->head=NULL;
        prhl->tail=NULL;
    }
}

int resources_handle_read(resources_handle_list *prhl){
    if(NULL == prhl){
        return -1;
    }
    char resbuf[128]={0};
    char path[64] = {0};
    char method[12] = {0};
    char *p = NULL;
    uint8_t *cmd = NULL;
    unsigned char *script = (g_scriptpoint ? g_scriptpoint : g_script); 
    asprintf(&cmd,"%s -l",script);
    FILE *fpp = popen(cmd,"r");
    free(cmd); 
    if(!fpp)
       return -1; 
    while(fgets(resbuf,128,fpp) != NULL)
    {   
       if( '\n' == resbuf[strlen(resbuf) - 1]) 
       {   
           resbuf[strlen(resbuf) - 1] = '\0';
       }else
       {   
           resbuf[strlen(resbuf)] = '\0';
       }   
       p = strchr(resbuf,','); 
       if (p){
           *p = 0;
           memcpy(path,resbuf,strlen(resbuf)+1); 
           memcpy(method,p+1,strlen(p+1)+1); 
           resources_handle_t *prh = (resources_handle_t *)malloc(sizeof(resources_handle_t));
           if(prh){
               memset(prh,0,sizeof(resources_handle_t));
               memcpy(prh->uri_path,path,strlen(path));
               if (0 == memcmp(method,"POST",4)){
                  prh->method = COAP_REQUEST_POST;
               }else if(0 == memcmp(method,"GET",3)){
                  prh->method = COAP_REQUEST_GET;
               }else if(0 == memcmp(method,"PUT",3)){
                  prh->method = COAP_REQUEST_PUT;
               }else if(0 == memcmp(method,"DELETE",6)){
                  prh->method = COAP_REQUEST_DELETE;
               }
               prh->hnd = resources_process_handle;
               add_resources_handle_list(prhl,prh);
            }
       }   
       memset(resbuf,0,128);
    }
    if(NULL == prhl->head){
        return -1;
    }
    return 0;
}

static void send_ack(coap_session_t *session,
                     coap_pdu_t *request,
                     unsigned char *data,
                     size_t len){
    if (NULL == data || 0 == len){
        return ;
    }
    coap_pdu_t *pdu=coap_pdu_init(COAP_MESSAGE_ACK,
                                  COAP_RESPONSE_CODE(205),
                                  request->tid,512);
    coap_add_data(pdu,len,data);
    coap_pdu_encode_header(pdu,COAP_PROTO_UDP);
    if(0 > coap_session_send_pdu(session,pdu)){
        debug("send ack failed\n"); 
    }
    coap_delete_pdu(pdu);
    pdu = NULL;
    return ;
}

int script_resource_process(uint8_t *path, uint8_t *info, 
                            uint8_t *data, uint8_t **result){
    int pfds[2];
    char *c = NULL;
    pid_t pid;
    
    if (pipe(pfds) < 0)
    	return -1;
    
    if (( pid = fork()) == -1)
    	goto error;
    
    if ( pid == 0) {
        /* child */
        const char *argv[8];
        int i = 0;
        unsigned char *script = (g_scriptpoint ? g_scriptpoint : g_script);
        if(data){
            //argv[i++] = "/bin/sh";
            argv[i++] = script;
            argv[i++] = "-p";
            argv[i++] = path;
            argv[i++] = "-i";
            argv[i++] = info;
            argv[i++] = "-d";
            argv[i++] = data;
            argv[i++] = NULL;
        }else{
            //argv[i++] = "/bin/sh";
            argv[i++] = script;
            argv[i++] = "-p";
            argv[i++] = path;
            argv[i++] = "-i";
            argv[i++] = info;
            argv[i++] = NULL;
        }
        close(pfds[0]);
        dup2(pfds[1], 1);
        close(pfds[1]);
        execvp(argv[0], (char **)argv);
        exit(0);
    } else if (pid < 0)
    	goto error;
    
    /* parent */
    close(pfds[1]);

    int status;
    while (wait(&status) != pid) {
        debug("waiting for child to exit\n");
    }
    
    char buffer[64];
    ssize_t rxed;
    int t;
    *result = NULL;
    while ((rxed = read(pfds[0], buffer, sizeof(buffer))) > 0) {
        if (*result)
            t = asprintf(&c, "%s%.*s", *result, (int) rxed, buffer);
        else
            t = asprintf(&c, "%.*s", (int) rxed, buffer);
        
        if (t == -1) goto error;

        free(*result);
        *result = strdup(c);
        free(c);
    }
    
    if (!(*result)) {
    	goto done;
    }

    if (!strlen(*result)){
        FREE(*result);
        goto done;
    }
    
    size_t i=0;
    uint8_t *fs = *result;
    for (i = strlen(fs) - 1; i > 0; i--){
        if ((fs[i] == '\r') || (fs[i] == '\n'))
            fs[i] = '\0';
        else
            break;
    }
    if (rxed < 0)
        goto error;
    
done:
    close(pfds[0]);
    return 0;

error:
    free(c);
    FREE(*result);
    close(pfds[0]);
    return -1;
}

void coap_add_response_data(coap_pdu_t *response,
                                    coap_session_t *session,
                                    uint8_t *sendbuf,
                                    size_t sendlen){
    if (sendlen > 0 ) {
        if (sendlen > (1u << (BLOCK_SZX + 4))){
            unsigned char buf[4];
            coap_add_option(response,
                      COAP_OPTION_BLOCK2,
                      coap_encode_var_bytes(buf,
                      (1u << 3) | BLOCK_SZX), buf);
            coap_add_option(response,28, //SIZE2
                                coap_encode_var_bytes(buf,sendlen),
                                buf);
            coap_add_block(response, sendlen, sendbuf, 0, BLOCK_SZX);
            coap_session_set_app_data(session,sendbuf);
        }
        else
            coap_add_data(response,sendlen,sendbuf);
    }
}

void 
resources_process_handle(coap_context_t *ctx UNUSED_PARAM,
              struct coap_resource_t *resource UNUSED_PARAM,
              coap_session_t *session UNUSED_PARAM,
              coap_pdu_t *request UNUSED_PARAM,
              str *token UNUSED_PARAM,
              str *query UNUSED_PARAM,
              coap_pdu_t *response){
              
    unsigned char buf[4];
    coap_opt_t *block_opt;
    coap_opt_iterator_t opt_iter;
    uint8_t *session_app = NULL;

    size_t recvlen = 0;
    uint8_t *recvbuf = NULL;
    uint8_t *recvbuftmp = NULL;
    uint8_t recvend = 0;
    
    block_opt = coap_check_option(request, COAP_OPTION_BLOCK1, &opt_iter);
    if(block_opt && COAP_OPTION_BLOCK1 == opt_iter.type){
         size_t num = coap_opt_block_num(block_opt);
         size_t szx = COAP_OPT_BLOCK_SZX(block_opt);
         size_t len = (1u << szx + 4);
         
         coap_get_data(request, &recvlen, &recvbuftmp); 
         if(0 < recvlen){
             recvbuf = (uint8_t *)malloc(recvlen + 1);
             memcpy(recvbuf,recvbuftmp,recvlen);
             recvbuf[recvlen] = 0;
         }
         session_app = NULL;
         session_app = (uint8_t *)coap_session_get_app_data(session);
         if(0 != num && NULL != session_app){
            len = strlen(session_app);
            uint8_t *newbuf = (uint8_t *)malloc(len + recvlen + 1);
            memcpy(newbuf,session_app,len);
            if(0 < recvlen){
                memcpy(newbuf+len,recvbuf,recvlen);
                free(recvbuf);
                recvbuf = NULL;
            }
            recvlen = len + recvlen;
            recvbuf = newbuf;
            recvbuf[recvlen] = 0;
            free(session_app);
            session_app = NULL;
         }else if(0 == num && NULL != session_app){
            free(session_app);
            session_app = NULL;
         }
         coap_session_set_app_data(session,(void *)NULL);
         
         if(COAP_OPT_BLOCK_MORE(block_opt)) {
              coap_session_set_app_data(session,(void *)recvbuf);
              coap_add_option(response,
                          COAP_OPTION_BLOCK1,
                          coap_encode_var_bytes(buf,
                          (num + 1 << 4) | szx), buf);
         }else{
            recvend = 1;
         }
    }else{
        block_opt = coap_check_option(request, COAP_OPTION_BLOCK2, &opt_iter);
        session_app = (uint8_t *)coap_session_get_app_data(session); 
        if(block_opt && COAP_OPTION_BLOCK2 == opt_iter.type ){
            unsigned int szx = COAP_OPT_BLOCK_SZX(block_opt);
            if (session_app){
                unsigned int num = coap_opt_block_num(block_opt);
                //debug("found Block2 option, block szx=%u, block num=%u\n", szx, num);
                unsigned int bytes_sent = ( num + 1) << (szx + 4); 
                unsigned int m = strlen(session_app) >  bytes_sent; 
                coap_add_option(response,
                                  COAP_OPTION_BLOCK2,
                                  coap_encode_var_bytes(buf,
                                  (num << 4) | (m << 3) | szx), buf);
       
                coap_add_block(response, strlen(session_app), session_app, num, szx);
                if(0 == m){
                    free(session_app);
                    session_app = NULL;
                    coap_session_set_app_data(session,(void *)NULL);
                    g_block_szx = BLOCK_SZX; 
                }
            }else{
                g_block_szx = szx; 
                coap_get_data(request, &recvlen, &recvbuftmp); 
                if(0 < recvlen){
                    recvbuf = (uint8_t *)malloc(recvlen + 1);
                    memcpy(recvbuf,recvbuftmp,recvlen);
                    recvbuf[recvlen] = 0;
                }
                recvend = 1;
            }
        }else{
            coap_get_data(request, &recvlen, &recvbuftmp); 
            if(0 < recvlen){
                recvbuf = (uint8_t *)malloc(recvlen + 1);
                memcpy(recvbuf,recvbuftmp,recvlen);
                recvbuf[recvlen] = 0;
            }
            recvend = 1;
        }
    }
    if(recvend)
    {
        size_t sendlen = 0;
        uint8_t *sendbuf = NULL;
        str *uri_path = coap_get_uri_path(request);
        uint8_t *path = (uint8_t *)malloc(uri_path->length + 1);
        memcpy(path,uri_path->s,uri_path->length);
        path[uri_path->length] = 0;

        char *client_info = NULL;
        char client_addr[64]={0};
        uint16_t client_port;
        debug("AF=%d,fam=%d\n",AF_INET,session->remote_addr.addr.sa.sa_family);
        if (AF_INET == session->remote_addr.addr.sa.sa_family){
            inet_ntop(AF_INET, &session->remote_addr.addr.sin.sin_addr, 
                                        client_addr, sizeof(client_addr));
            client_port = ntohs(session->remote_addr.addr.sin.sin_port);
        }
        else if (AF_INET6 == session->remote_addr.addr.sa.sa_family){
            inet_ntop(AF_INET6, &session->remote_addr.addr.sin6.sin6_addr, 
                                        client_addr, sizeof(client_addr));
            client_port = ntohs(session->remote_addr.addr.sin6.sin6_port);

        }
        //if(0 != memcmp("device/command/file",path,min(strlen(path),19))){
            asprintf(&client_info,"{\"ip\":\"%s\",\"port\":\"%d\"}",
                                   client_addr,client_port);
            debug("%s\n",client_info);
            script_resource_process(path,client_info,recvbuf,&sendbuf);
            free(client_info);
        /*
        }else{
            asprintf(&client_info,"{\"order\":1}");
            debug("%s\n",client_info);
            script_resource_process(path,client_info,recvbuf,&sendbuf);
            free(client_info);
            if(sendbuf && 0 == memcmp("one",sendbuf,3)){
                 unsigned char order[64] = "{\"order\":2}";
                 unsigned char res[64]="{\"respCode\":0,\"respCont\":\"Received Success\"}";
                 send_ack(session,request,res,strlen(res));
                 unsigned char trans[64]="{\"respCode\":2002,\"respCont\":\"File Stransferring\"}";
                 send_ack(session,request,trans,strlen(trans));
                 free(sendbuf);
                 sendbuf = NULL;
                 script_resource_process(path,order,recvbuf,&sendbuf);
            }
            if(sendbuf && 0 == memcmp("two",sendbuf,3)){
                 unsigned char order[64] = "{\"order\":3}\0";
                 unsigned char res[64]="{\"respCode\":2002,\"respCont\":\"File Stransferring\"}";
                 send_ack(session,request,res,strlen(res));
                 free(sendbuf);
                 sendbuf = NULL;
                 script_resource_process(path,order,recvbuf,&sendbuf);
            }
        }
        */
        if(recvbuf){
            free(recvbuf);
        }
        free(path);
        if(sendbuf){
            sendlen = strlen(sendbuf);
        }
        if (sendlen > 0 ) {
            if (sendlen > (1u << (g_block_szx + 4))){
                unsigned char buf[4];
                coap_add_option(response,
                          COAP_OPTION_BLOCK2,
                          coap_encode_var_bytes(buf,
                          (1u << 3) | g_block_szx), buf);
                coap_add_option(response,28, //SIZE2
                                    coap_encode_var_bytes(buf,sendlen),
                                    buf);
                coap_add_block(response, sendlen, sendbuf, 0, g_block_szx);
                coap_session_set_app_data(session,sendbuf);
            } else{
                coap_add_data(response,sendlen,sendbuf);
                free(sendbuf);
            }
        }
    }
    response->code = COAP_RESPONSE_CODE(205);
}

