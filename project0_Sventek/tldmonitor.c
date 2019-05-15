#include "date.h"
#include "tldmap.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define USAGE "usage: %s begin_datestamp end_datestamp [file] ...\n"
#define ERROR_EXIT 1

static void fold(char *s) {
    int i;

    for (i = 0; s[i] != '\0'; i++) {
        s[i] = tolower((int)s[i]);
    }
}

static char *extract(char *hostname) {
    char *p = strrchr(hostname, '.');
    if (p == NULL)
        p = hostname;
    else
        p++;
    return p;
}

static void process(FILE *fd, const Date *begin, const Date *end,
                    const TLDMap *tld, long *count) {
    char bf[1024], sbf[1024];
    const Date *d;
    char *theTLD;
    while (fgets(bf, sizeof(bf), fd) != NULL) {
        char *q, *p = strchr(bf, ' ');
	if (p == NULL) {
            fprintf(stderr, "Illegal input line: %s", bf);
	    return;
        }
	strcpy(sbf, bf);
	*p++ = '\0';
	while (*p == ' ')
            p++;
	q = strchr(p, '\n');
	if (q == NULL) {
            fprintf(stderr, "Illegal input line: %s", sbf);
	    return;
        }
	*q = '\0';
        fold(p);
        theTLD = extract(p);
	d = Date_create(bf);
        if (!(d->compare(d, end) > 0 || d->compare(d, begin) < 0)) {
            long value;
            *count += 1;
            if (! tld->lookup(tld, theTLD, &value))
                tld->insert(tld, theTLD, 1L);
            else
                tld->reassign(tld, theTLD, value+1);
        }
	d->destroy(d);
    }
}

int main(int argc, char *argv[]) {
    const Date *begin, *end;
    int i;
    FILE *fd;
    const TLDMap *tld = NULL;
    const Iterator *it = NULL;
    TLDNode *n;
    double total;
    long count = 0;

    if (argc < 3) {
        fprintf(stderr, USAGE, argv[0]);
        return ERROR_EXIT;
    }
    begin = Date_create(argv[1]);
    if (begin == NULL) {
        fprintf(stderr, "Error processing begin date: %s\n", argv[1]);
        return ERROR_EXIT;
    }
    end = Date_create(argv[2]);
    if (end == NULL) {
        fprintf(stderr, "Error processing end date: %s\n", argv[2]);
        goto error;
    }
    if (begin->compare(begin, end) > 0) {
        fprintf(stderr, "%s > %s\n", argv[1], argv[2]);
	goto error;
    }
    tld = TLDMap_create();
    if (tld == NULL) {
        fprintf(stderr, "Unable to create TLD list\n");
        goto error;
    }
    if (argc == 3)
        process(stdin, begin, end, tld, &count);
    else {
        for (i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-") == 0)
                fd = stdin;
            else
                fd = fopen(argv[i], "r");
            if (fd == NULL) {
                fprintf(stderr, "Unable to open %s\n", argv[i]);
                continue;
            }
            process(fd, begin, end, tld, &count);
            if (fd != stdin)
                fclose(fd);
        }
    }
    total = (double)count;
    it = tld->itCreate(tld);
    if (it == NULL) {
        fprintf(stderr, "Unable to create iterator\n");
        goto error;
    }
    while (it->hasNext(it)) {
        (void) it->next(it, (void **)&n);
        printf("%6.2f %s\n", 100.0 * (double)TLDNode_count(n)/total, TLDNode_tldname(n));
    }
    it->destroy(it);
    tld->destroy(tld);
    begin->destroy(begin);
    end->destroy(end);
    return 0;
error:
    if (it != NULL)	it->destroy(it);
    if (tld != NULL)	tld->destroy(tld);
    if (end != NULL)	end->destroy(end);
    if (begin != NULL)	begin->destroy(begin);
    return ERROR_EXIT;
}
