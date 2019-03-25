SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
LIBA=libaescbc128.a
TARGET=aescbc128

CFLAGS += -g -O2

all:$(TARGET) $(LIBA)

$(TARGET):$(LIBA)
	$(CC) -L.  $^ -o $@ -laescbc128

$(LIBA):$(OBJS)
	$(AR) cr $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o  $(OBJS) $(LIBA) $(TARGET)
