SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROG=server client audio_test
INCLUDE = common.h

CFLAGS += $(INCLUDE:%=-include %)
CFLAGS += -Wall -Wextra -pedantic -std=gnu99
CFLAGS += -O3 -march=native -flto

all: $(PROG)

server: server.o utils.o audio.o
server: LDFLAGS += -lportaudio
server: INCLUDE += audio.h
client: client.o utils.o audio.o
client: LDFLAGS += -lportaudio
client: INCLUDE += audio.h
audio_test: audio.o
audio_test: LDFLAGS += -lportaudio
audio_test: INCLUDE += audio.h

clean:
	rm -rf $(wildcard $(PROG)) $(wildcard $(OBJS))
