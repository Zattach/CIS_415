#include "queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define UNUSED __attribute__((unused))

int main(UNUSED int argc, UNUSED char *argv[]) {
    char buf[1024];
    char *p, *q;
    const Queue *uq;
    long i, n;
    const Iterator *it;

    if ((uq = Queue_create(0L)) == NULL) {
        fprintf(stderr, "Error creating queue of strings\n");
        return -1;
    }
 
    /*
     * test of enqueue()
     */
    while (fgets(buf, 1024, stdin) != NULL) {
        if ((p = strdup(buf)) == NULL) {
            fprintf(stderr, "Error duplicating string\n");
            return -1;
        }
        if (!uq->enqueue(uq, p)) {
            fprintf(stderr, "Error enqueueing string to queue\n");
            return -1;
        }
    }
    /*
     * test of front and dequeue()
     */
    i = 0;
    n = uq->size(uq);
    for (i = 0; i < n; i++) {
        if (!uq->front(uq, (void **)&p)) {
            fprintf(stderr, "Error retrieving %ld'th element\n", i);
            return -1;
        }
        if (!uq->dequeue(uq, (void **)&q)) {
            fprintf(stderr, "Error dequeueing %ld'th element\n", i);
            return -1;
        }
        if (strcmp(p, q) != 0) {
            fprintf(stderr, "Returns from front and dequeue are not the same\n");
            return -1;
        }
        if (!uq->enqueue(uq, p)) {
            fprintf(stderr, "Unable to requeue %ld'th element\n", i);
            return -1;
        }
    }
    /*
     * test of iterator
     */
    if ((it = uq->itCreate(uq)) == NULL) {
        fprintf(stderr, "Error in creating iterator\n");
        return -1;
    }
    while (it->hasNext(it)) {
        char *p;
        (void) it->next(it, (void **)&p);
        printf("%s", p);
    }
    it->destroy(it);
    uq->destroy(uq, free);

    return 0;
}

