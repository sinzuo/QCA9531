SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
LIBA=libcjson.a
TARGET=test

CFLAGS += -O2

all:$(TARGET) $(LIBA)

$(TARGET):$(LIBA)
	$(CC) -L.  $^ -o $@ -lcjson

$(LIBA):$(OBJS)
	$(AR) cr $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o  $(OBJS) $(LIBA) $(TARGET)
