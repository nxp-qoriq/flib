PROJECTS = sec/rta

all:
	for dir in $(PROJECTS); do \
        $(MAKE) -C $$dir;\
	done
clean:
	for dir in $(PROJECTS); do \
        $(MAKE) -C $$dir clean;\
	done

install:
	for dir in $(PROJECTS); do \
        $(MAKE) -C $$dir install;\
	done
