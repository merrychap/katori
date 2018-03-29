CC     = gcc
CFLAGS = -D_REENTRANT -Wall -pedantic -Isrc
LDLIBS = -lpthread

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

TARGETS = tests/thread_pool_test src/main
# libthreadpool.so libthreadpool.a

all: $(TARGETS)

src/thread_pool.o: src/thread_pool.c src/thread_pool.h
src/server.o: src/server.c src/server.h
src/main.o: src/main.c src/server.h

tests/thread_pool_test.o: tests/thread_pool_test.c src/thread_pool.h

tests/thread_pool_test: tests/thread_pool_test.o src/thread_pool.o
src/main: src/main.o src/server.o

.PHONY: clean
clean:
	rm -f $(TARGETS) *~ */*~ */*.o

test: $(TARGETS)
	./tests/thread_pool_test