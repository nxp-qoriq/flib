RTA_DIR=../test_install/usr/include

CFLAGS=-Wall -Wshadow -O3 -g
CFLAGS+=-I${RTA_DIR}

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=)


.PHONY: all

all: $(OBJS)


.PHONY: clean

clean:
	rm -f $(OBJS)


$(SRCS):
	${CROSS_COMPILE}gcc -o $@ $^
