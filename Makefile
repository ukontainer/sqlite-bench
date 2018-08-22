CFLAGS=-Wall -I. -O0 -g
LDFLAGS=-lpthread -ldl -lm
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
HDRS=$(wildcard *.h)
TARGET=sqlite-bench

$(TARGET): $(OBJS)
	cc -o $@ $^ $(LDFLAGS)

$(OBJS): $(HDRS)

clean:
	rm -f $(TARGET) *.o

.PHONY: clean
