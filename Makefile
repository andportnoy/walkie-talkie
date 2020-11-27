CFLAGS+=-include project.h
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROG=main

CFLAGS += -O3 -Wall -Wextra -std=gnu99 -march=native -flto
LDFLAGS = -lportaudio

$(PROG): $(OBJS)

clean:
	rm -rf $(PROG) $(OBJS)
