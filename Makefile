CFLAGS=-Wall -I. -O2 -DNDEBUG -std=c99
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
HDRS=$(wildcard *.h)
TARGET=sqlite-bench

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LDFLAGS=-lpthread -ldl -lm
else
	LDFLAGS=-lpthread -ldl -lm -static
endif


$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o : %.c 
	$(CC) -c $<

$(OBJS): $(HDRS)

bench: $(TARGET) clean-db
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

clean-db:
	rm -f dbbench_sqlite3*

.PHONY: bench clean clean-db
