SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
EXEC += clientprobe

CFLAGS += -DDEBUG

all: $(EXEC) 

$(EXEC):$(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -luci -lubox

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

