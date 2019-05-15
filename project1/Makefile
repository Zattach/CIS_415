CFLAGS=-W -Wall
PROGRAMS=uspsv1 cpubound iobound

all: $(PROGRAMS)

uspsv1: uspsv1.o queue.o iterator.o p1fxns.o
	gcc -o uspsv1 $(CFLAGS) $^

cpubound: cpubound.o
	gcc -o cpubound $(CFLAGS) $^

iobound: iobound.o
	gcc -o iobound $(CFLAGS) $^

clean:
	rm -f *.o $(PROGRAMS)

uspsv1.o: uspsv1.c
queue.o: queue.c queue.h
iterator.o: iterator.c iterator.h
p1fxns.o: p1fxns.c p1fxns.h
cpubound.o: cpubound.c
iobound.o: iobound.c
