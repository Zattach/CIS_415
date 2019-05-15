CFLAGS= -W -Wall -g -std=c11 -D_DEFAULT_SOURCE 
PROGS=include_crawler
OBJECTS=tshashmap.o hashmap.o include_crawler.o tslinkedlist.o linkedlist.o tsiterator.o iterator.o
LIBS=-lpthread

include_crawler: include_crawler.o tshashmap.o hashmap.o tslinkedlist.o linkedlist.o tsiterator.o iterator.o
	gcc $(LIBS) -o include_crawler $^ 


include_crawler.o: include_crawler.c
tshashmap.o: tshashmap.c tshashmap.h
hashmap.o: hashmap.c hashmap.h
tslinkedlist.o: tslinkedlist.c tslinkedlist.h
linkedlist.o: linkedlist.c linkedlist.h
tsiterator.o: tsiterator.c tsiterator.h
iterator.o: iterator.c iterator.h


clean:
	rm -f *~ $(OBJECTS) $(PROGS)