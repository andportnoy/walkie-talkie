SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROG=ptt gui
INCLUDE = common.h audio.h network.h utils.h

CFLAGS += $(INCLUDE:%=-include %)
CFLAGS += -Wall -Wextra -pedantic -std=gnu99
CFLAGS += -O3 -march=native -flto

LDFLAGS += -lportaudio -lpthread

all: $(PROG)
gui: INCLUDE += gtk/gtk.h
gui: CFLAGS  += $(shell pkg-config --cflags gtk+-3.0)
gui: LDFLAGS += $(shell pkg-config --libs   gtk+-3.0)
$(OBJS): $(INCLUDE)

ptt: network.o audio.o utils.o

clean:
	rm -rf $(wildcard $(PROG)) $(wildcard $(OBJS))
