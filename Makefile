SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROG=server client audio

CFLAGS += -include project.h
CFLAGS += -O3 -Wall -Wextra -pedantic -std=gnu99 -march=native -flto

all: $(PROG)
server: server.o utils.o
client: client.o utils.o
audio: LDFLAGS += -lportaudio

clean:
	rm -rf $(wildcard $(PROG)) $(wildcard $(OBJS))
