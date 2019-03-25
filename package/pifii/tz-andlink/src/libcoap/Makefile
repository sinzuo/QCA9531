# Makefile  
SOURCES=$(wildcard src/*.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
#DIRS=$(patsubst %.c,%.o,$(SOURCES))
#OBJS=$(notdir $(DIRS))

LIBSO=libcoap.so
LIBA=libcoap.a
WARNING_CFLAGS = -pedantic -Wall -Wextra -Wformat-security -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wshadow -Wstrict-prototypes -Wswitch-default -Wswitch-enum  -Wunused  -Wlogical-op -Wunused-result
CFLAGS += $(WARNING_CFLAGS) 
CFLAGS += -g -O2 -D_GNU_SOURCE -Iinclude/coap
CFLAGS += -DNDEBUG 
#CFLAGS += -g -O2 -fdiagnostics-color -D_GNU_SOURCE -Iinclude/coap
#CFLAGS += -std=c99 -fPIC -DPIC 
#$(RANLIB) $@
  
#VPATH  = src:include/coap 
vpath %.h include/coap 
vpath %.c src  
  
all:$(LIBA)

$(LIBSO):$(OBJS)
	$(LD) -shared -o $@ $^
  
$(LIBA):$(OBJS)
	$(AR) cr $@ $^

%.o:%.c  
	$(CC) $(CFLAGS) -c $< -o $@ 
  
.PHONY:clean  
clean:  
	-rm -f $(LIBSO) $(LIBA) $(OBJS)
