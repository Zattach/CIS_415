CFLAGS= -std=c89 -W -Wall -g
PROGS= tldmonitorLL tldmonitor
OBJECTS=date.o tldmap.o

all: $(PROGS)

tldmonitorLL: tldmonitor.o date.o tldmapll.o iterator.o
	cc -o tldmonitorLL $^

tldmonitor: tldmonitor.o date.o tldmap.o iterator.o
	cc -o tldmonitor $^

tldmonitor.o: tldmonitor.c date.h tldmap.h
date.o: date.h date.c
tldmap.o: tldmap.c tldmap.h date.h
iterator.o: iterator.c iterator.h

clean:
	rm -f *~ $(OBJECTS) $(PROGS)
