CC=$(CROSS_COMPILE)gcc
INCLUDES=-I../include
CFLAGS=-Wall -Wshadow -O3 -g $(INCLUDES)
CPPFLAGS=-DDEBUG

ALL_SRCS=$(wildcard *.c)
LIB_SRCS=test_common.c
LIB_OBJS=$(LIB_SRCS:.c=.o)
LIBS=$(LIB_SRCS:.c=)
TEST_SRCS=$(filter-out $(LIB_SRCS), $(ALL_SRCS))
TEST_OBJS=$(TEST_SRCS:.c=.o)
TESTS=$(TEST_SRCS:.c=)
DEPENDS=$(ALL_SRCS:.c=.d)
LOGS=$(ALL_SRCS:.c=.desc)

.PHONY: all check clean

all: $(TESTS)

# Build dependencies (*.d files) for source (*.c) files.
# Use the "-MM" compiler flag (ignores system headers).
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# No need to recreate *.d files when cleaning up.
# Also, when building only one test, create dependencies only for that test.
ifneq ($(MAKECMDGOALS), clean)
ifneq (,$(filter $(MAKECMDGOALS), all check))
-include $(DEPENDS)
else
-include $(MAKECMDGOALS).d $(LIB_SRCS:.c=.d)
endif
endif

# Run all tests and save the logs in *.desc files.
# Further, these files can be diff-ed against DXLT output.
check: $(TESTS)
	@echo Running tests...
	@for test in $^ ; do \
		./$$test > $$test.desc ; \
	done
	@echo Done! Output logged in \*.desc files.

$(TESTS): % : %.o $(LIB_OBJS)
	$(CC) -o $@ $^

# We don't want to automatically build the libraries (.o -> elf).
# Thus we have to cancel implicit build rules by overriding them
# using an empty recipe.
$(LIBS): $(LIB_OBJS)
	

clean:
	@rm -f $(TESTS) $(TEST_OBJS) $(LIB_OBJS) $(DEPENDS) $(LOGS)

