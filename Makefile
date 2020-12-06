SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROG=server client audio_test
INCLUDE = common.h audio.h network.h

CFLAGS += $(INCLUDE:%=-include %)
CFLAGS += -Wall -Wextra -pedantic -std=gnu99
CFLAGS += -O3 -march=native -flto

LDFLAGS += -lportaudio -lpthread

all: $(PROG)
$(OBJS): $(INCLUDE)

server: network.o audio.o
client: network.o audio.o
audio_test: audio.o

clean:
	rm -rf $(wildcard $(PROG)) $(wildcard $(OBJS))
