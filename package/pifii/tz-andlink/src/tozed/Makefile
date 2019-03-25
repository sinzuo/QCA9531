#Makefile

#CFLAGS = -g -O2 -fdiagnostics-color -D_GNU_SOURCE
CFLAGS = -g -O2 -D_GNU_SOURCE
CFLAGS += -I../libcoap/include
CFLAGS += -DNDEBUG 

LIBDIR = -L../libcoap

CLIENT=coap-client
CLIENT_OBJS=coap-client.o coap_list.o

ANDLINK_SERVER=coap-server
ANDLINK_SERVER_OBJS=coap-server.o coap_list.o handle.o

B_CLIENT=coap-broadcast
B_CLIENT_OBJS=coap-broadcast.o

P_CLIENT=coap-ping
P_CLIENT_OBJS=coapping.o

all:$(CLIENT) $(ANDLINK_SERVER) $(B_CLIENT) $(P_CLIENT)

$(CLIENT):$(CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIBDIR) -lcoap

$(ANDLINK_SERVER):$(ANDLINK_SERVER_OBJS)
	$(CC) -o $@ $^ $(LIBDIR) -lcoap

$(B_CLIENT):$(B_CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIBDIR) -lcoap

$(P_CLIENT):$(P_CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIBDIR) -lcoap

%.o:%.c
	$(CC) $(CFLAGS) -c $<  

clean:  
	-rm -f $(CLIENT) $(CLIENT_OBJS) $(ANDLINK_SERVER) $(ANDLINK_SERVER_OBJS) $(B_CLIENT) $(B_CLIENT_OBJS) $(P_CLIENT) $(P_CLIENT_OBJS)
