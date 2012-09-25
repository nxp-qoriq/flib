DESTDIR ?= $(shell pwd)/test_install

HEADERS := $(wildcard *.h)
TOP_HEADERS := desc.h protoshared.h rta.h
IN_HEADERS := $(filter-out $(TOP_HEADERS),$(HEADERS))

.PHONY: all
all: ;

.PHONY:	install
install:
	install -m 755 -d $(DESTDIR)/usr/include/flib/rta

	for header in $(TOP_HEADERS); do \
	install -m 0644 $$header $(DESTDIR)/usr/include/flib; \
	done

	for header in $(IN_HEADERS); do \
	install -m 0644 $$header $(DESTDIR)/usr/include/flib/rta; \
	done

.PHONY: clean
clean: ;
