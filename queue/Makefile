CFLAGS=-W -Wall
OBJECTS=queue.o iterator.o

test: test.o $(OBJECTS)
	gcc -o test $^

clean:
	rm -f test test.o $(OBJECTS)

test.o: test.c queue.h
queue.o: queue.c queue.h iterator.h
iterator.o: iterator.c iterator.h
