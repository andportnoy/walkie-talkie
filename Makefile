SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROG=ptt
INCLUDE = common.h audio.h network.h utils.h

CFLAGS += $(INCLUDE:%=-include %)
CFLAGS += -Wall -Wextra -pedantic -std=gnu99
CFLAGS += -O3 -march=native -flto

LDFLAGS += -lportaudio -lpthread

all: $(PROG)
$(OBJS): $(INCLUDE)

ptt: network.o audio.o

clean:
	rm -rf $(wildcard $(PROG)) $(wildcard $(OBJS))
