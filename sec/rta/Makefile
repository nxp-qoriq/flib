# Copyright 2008-2013 Freescale Semiconductor, Inc.

prefix=/usr
includedir=$(prefix)/include


.PHONY: all
all: ;

.PHONY:	install
install:
	install -m 755 -d $(DESTDIR)$(includedir)/flib/rta
	cd include/flib &&\
	install -m 0644 *.h $(DESTDIR)$(includedir)/flib
	cd include/flib/rta &&\
	install -m 0644 *.h $(DESTDIR)$(includedir)/flib/rta

.PHONY: clean
clean: ;
