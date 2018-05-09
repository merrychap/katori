CC     = gcc
CFLAGS = -D_REENTRANT -Wall -pedantic -Isrc
LDLIBS = -lpthread

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

TARGETS = tests/thread_pool_test src/main

all: $(TARGETS)

src/sniffing_utils.o: src/sniffing_utils.c src/sniffing_utils.h
src/cli_utils.o:      src/cli_utils.c src/cli_utils.h
src/sniffing.o:       src/sniffing.c src/sniffing.h src/cli_utils.h src/server.h
src/spoofing.o:       src/spoofing.c src/spoofing.h src/cli_utils.h
src/cli.o:            src/cli.c src/cli.h src/cli_utils.h src/sniffing.h src/spoofing.h
src/thread_pool.o:    src/thread_pool.c src/thread_pool.h
src/server.o: 	      src/server.c src/server.h src/thread_pool.h src/sniffing_utils.h
src/main.o: 	      src/main.c src/server.h src/thread_pool.h

tests/thread_pool_test.o: tests/thread_pool_test.c src/thread_pool.h

src/main: src/main.o src/server.o src/thread_pool.o src/cli.o src/sniffing.o src/spoofing.o src/cli_utils.o src/sniffing_utils.o

tests/thread_pool_test: tests/thread_pool_test.o src/thread_pool.o

.PHONY: clean
clean:
	rm -f $(TARGETS) *~ */*~ */*.o

test: $(TARGETS)
	./tests/thread_pool_test