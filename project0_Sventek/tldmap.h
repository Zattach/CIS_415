/*
 * Copyright (c) 2017, University of Oregon
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the University of Oregon nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
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
