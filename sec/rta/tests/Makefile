CFLAGS=-Wall -Wshadow -O3 -g
CFLAGS+=-I../include

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=)


.PHONY: all

all: $(OBJS)


.PHONY: clean

clean:
	rm -f $(OBJS)


$(SRCS):
	${CROSS_COMPILE}gcc -o $@ $^
