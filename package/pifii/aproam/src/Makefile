SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
EXEC += getterminal

CFLAGS += -DDEBUG

all: $(EXEC) 

$(EXEC):$(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

