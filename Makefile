CFLAGS=-Wall -I. -O2 -DNDEBUG -std=c99
LDFLAGS=-lpthread -ldl -lm -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
HDRS=$(wildcard *.h)
TARGET=sqlite-bench

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJS): $(HDRS)

bench: $(TARGET) clean-db
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

clean-db:
	rm -f dbbench_sqlite3*

.PHONY: bench clean clean-db
