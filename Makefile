CFLAGS=-Wall -I. -O0 -g
LDFLAGS=-lpthread -ldl -lm
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
HDRS=$(wildcard *.h)
TARGET=sqlite-bench

$(TARGET): $(OBJS)
	cc -o $@ $^ $(LDFLAGS)

$(OBJS): $(HDRS)

bench: $(TARGET) clean-db
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

clean-db:
	rm -f dbbench_sqlite3*

.PHONY: bench clean clean-db
