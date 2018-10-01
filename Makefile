
PREFIX = ${DESTDIR}/usr
BINDIR = ${PREFIX}/sbin
BINDIR_ALT = ${PREFIX}/bin

LIBS = libevent libbsd

CFLAGS = -Wall -D_GNU_SOURCE `pkg-config --cflags ${LIBS}`
LDFLAGS = -Wl,--as-needed `pkg-config --libs ${LIBS}`

OBJ_KATORI = src/katori.o src/cli_utils.o src/cli.o \
			 src/log.o src/netlistener.o src/sniffer.o \
			 src/packet.o src/spoofing.o src/utils.o \
			 src/argparser.o src/sniffer_cli.o

TARGETS = katori

CPPFLAGS = -DDEBUG

all: ${TARGETS}

katori: ${OBJ_KATORI}
	${CC} ${OBJ_KATORI} ${LDFLAGS} -o katori


.PHONY: clean
clean:
	rm -f $(TARGETS) *~ */*~ */*.o

test: $(TARGETS)
	./tests/thread_pool_test