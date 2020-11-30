CFLAGS+=-include project.h
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROG=server client

CFLAGS += -O3 -Wall -Wextra -pedantic -std=gnu99 -march=native -flto

all: $(PROG)
server: server.o utils.o
client: client.o utils.o

clean:
	rm -rf $(PROG) $(OBJS)
