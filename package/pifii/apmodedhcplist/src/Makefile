SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
EXEC += apmodedhcplist

CFLAGS += -DDEBUG

all: $(EXEC) 

$(EXEC):$(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lpcap

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

