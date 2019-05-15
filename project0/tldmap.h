#ifndef _TLDMAP_H_INCLUDED_
#define _TLDMAP_H_INCLUDED_

#include "date.h"
#include "iterator.h"

typedef struct tldmap TLDMap;
typedef struct tldnode TLDNode;

/*
 * TLDMap_create generates a map structure for storing counts against
 * top level domains (TLDs)
 *
 * creates an empty TLDMap
 * returns a pointer to the list if successful, NULL if not
 */
const TLDMap *TLDMap_create(void);

struct tldmap {
    void *self;               /* instance specific data */
/*
 * destroy() destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the map is returned to the heap
 */
    void (*destroy)(const TLDMap *tld);

/*
 * insert() inserts the key-value pair (theTLD, v) into the map;
 * returns 1 if the pair was added, returns 0 if there already exists en
 * entry corresponding to TLD
 */
    int (*insert)(const TLDMap *tld, char *theTLD, long v);

/*
 * reassign() replaces the current value corresponding to theTLD in the
 * map with v.
 * returns 1 if the value was replaced, returns 0 if no such key exists
 */
    int (*reassign)(const TLDMap *tld, char *theTLD, long v);

/*
 * lookup() returns the current value associated with theTLD in *v
 * returns 1 if the lookup was successful, returns 0 if no such key exists
 */
    int (*lookup)(const TLDMap *tld, char *theTLD, long *v);

/*
 * itCreate() creates an iterator over the map
 * returns the iterator if successful, NULL if unsuccessful
 */
    const Iterator *(*itCreate)(const TLDMap *tld);
};

/*
 * TLDNode_tldname returns the tld associated with the TLDNode
 */
char *TLDNode_tldname(TLDNode *node);

/*
 * TLDNode_count returns the value currently associated with that TLD
 */
long TLDNode_count(TLDNode *node);

#endif /* _TLDMAP_H_INCLUDED_ */
