CC     = gcc
CFLAGS = -D_REENTRANT -Wall -pedantic -Isrc
LDLIBS = -lpthread

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

TARGETS = tests/thread_pool_test \
	libthreadpool.so libthreadpool.a

all: $(TARGETS)

src/thread_pool.o: src/thread_pool.c src/thread_pool.h
tests/thread_pool_test.o: tests/thread_pool_test.c src/thread_pool.h
tests/thread_pool_test: tests/thread_pool_test.o src/thread_pool.o

# shared: libthreadpool.so
# static: libthreadpool.a

libthreadpool.so: src/thread_pool.c src/thread_pool.h
	$(CC) -shared -fPIC ${CFLAGS} -o $@ $< ${LDLIBS}

src/libthreadpool.o: src/thread_pool.c src/thread_pool.h
	$(CC) -c -fPIC ${CFLAGS} -o $@ $<

libthreadpool.a: src/libthreadpool.o
	ar rcs $@ $^

.PHONY: clean
clean:
	rm -f $(TARGETS) *~ */*~ */*.o

test: $(TARGETS)
	./tests/thread_pool_test