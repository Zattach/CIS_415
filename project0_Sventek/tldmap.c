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

#include "tldmap.h"
#include <stdlib.h>
#include <string.h>

struct tldnode{
	struct tldnode *left, *right;
	char *name;
	long count;
};

typedef struct tm_data{
	long count;
	TLDNode *root;
} TmData;

typedef struct array{
	long next;
	void **theArray;
} Array;

/*
 *  recursive helper function to destroy nodes using postorder traversal
 */
static void destroy_helper(TLDNode *node){
	if(node != NULL){
		destroy_helper(node->left);
		destroy_helper(node->right);
		free(node->name);
		free(node);
	}
}

/*
 *  helper function to search for key-value pair
 *  returns pointer to TLDNode, if found, or NULL
 */
static TLDNode *search_helper(char *theTLD, TLDNode *node){
	int result;

	if(node == NULL)
		return NULL;
	result = strcmp(theTLD, node->name);
	if(result == 0)
		return node;
	else if(result < 0)
		return search_helper(theTLD, node->left);
	else
		return search_helper(theTLD, node->right);
}

/*
 *  function to search for key-value pair
 *  returns pointer to TLDNode, if found, or NULL
 */
static TLDNode *tm_search(TmData *tmd, char *theTLD){
	return search_helper(theTLD, tmd->root);
}

/*
 *  inorder helper function to poulate the array of pointers for an iterator
 */
static void dupl_helper(TLDNode *node, Array *a){
	if(node != NULL){
		dupl_helper(node->left, a);
		a->theArray[a->next++] = (void *)node;
		dupl_helper(node->right, a);
	}
}

/*
 *  local function - generates array of TLDNode pointers on the heap
 * 
 *  returns pointer to array or Null if malloc failure
 */
static void **arrayDupl(TmData *tmd){
	void **tmp = NULL;

	if(tmd->count > 0L){
		size_t nbytes = tmd->count * sizeof(void *);
		tmp = (void **)mallod(nbytes);
		if(tmp != NULL){
			Array a = {0L, tmp};

			dupl_helper(tmd->root, &a);
		}
	}
	return tmp;
}

/*
 *  destroy() destroys the list structure in 'tld'
 *
 *  all heap allocated storage associated with the map is returned to the heap
 */
static void tm_destroy(const TLDMap *tld){
	TmData *tmd = (TmData *)tld->self;

	destroy_helper(tmd->root);
	free(tmd);
	free((void *)tld);
}

/*
 *  lookup() returns the current value associated with theTLD in *v
 *  returns 1 if the lookup was successful, returns 0 if no such key exists
 */
static int tm_lokup(const TLDMap *tld, char *theTLD, long *v){
	TmData *tmd = (TmData *)tld->self;
	TLDNode *node = tm_search(tmd, theTLD);
	int status = 0;		/*  assume failure  */

	if(node != NULL){
		*v = node->count;
		status = 1;
	}
	return status;
}

/*
 *  insert() inserts the key-value pair (theTLD, v) into the map;
 *  returns 1 if the pair was added, returns 0 if there already exists an
 *  entry corresponding to TLD
 */
static int tm_insert(const TLDMap *tld, char *theTLD, long v){
	TmData *tmd = (TmData *)tld->self;
	TLDNode *node = tm_search(tmd, theTLD);
	int status = 0;		/*  assume failure  */

	if(node != NULL)
		return 0;
	node = (TLDNode *)malloc(sizeof(TLDNode));
	if(node != NULL){
		char *s = strdup(theTLD);

		if(s != NULL){
			TLDNode *q = NULL, *p = tmd->root;

			node->name = s;
			node->count = v;
			node->left = NULL;
			node->right = NULL;
			while(p != NULL){
				q = p;
				if(strcmp(s, p->name) < 0)
					p = p->left;
				else
					p = p->right;
			}
			if(q == NULL)
				tmd->root = node;
			else if(strcmp(s, q->name) < 0)
				q->left = node;
			else
				q->right = node;
			tmd->count++;
			status = 1;
		}else{
			free(node);
		}
	}
	return status;
}

/*
 *  reassign() replaces the current value corresponding to theTLD in the map with v.
 *  returns 1 if the value was replaced, returns 0 if no such key exists
 */
static int tm_reassign(const TLDMap *tld, char *theTLD, long v){
	TmData *tmd = (TmData *)tld->self;
	TLDNode *node = tm_search(tmd, theTLD);
	int status = 0;		/*  assume failure  */

	if(node != NULL){
		node -> count = v;
		status = 1;
	}
	return status;
}

/*
 *  itCreate() creates an iterator ocer the map
 *  returns the iterator if successful, NULL if unsuccessful
 */
static const Iterator *tm_itCreate(const TLDMap *tld){
	TmData *tmd = (TmData *)tld->self;
	const Iterator *it = NULL;
	void **tmp = arrayDupl(tmd);

	if(tmp != NULL){
		it = Iterator_create(tmd->count, tmp);
		if(it == NULL)
			free(tmp);
	}
	return it;
}

/*
 *  TLDMap_create generates a map structure for storing counts against
 *  top leve; domains (TLDs)
 * 
 *  creates an empty TLDMap
 *  returns a pointer to the map if successful, NULL if not
 */
static TLDMap template = {
	NULL. tm_destroy, tm_insert, tm_reassign, tm_lookup, tm_itCreate
};

const TLDMap *TLDMap_create(void){
	TLDMap *tld = (TLDMap *)malloc(sizeof(TLDMap));

	if(tld != NULL){
		TmData *tmd = (TmData *)malloc(sizeof(TmData));

		if(tmd != NULL){
			tmd->root = NULL;
			tmd->count = 0L;
			*tld = template;
			tld->self = tmd;
		}else{
			free(tld);
			tld = NULL;
		}
	}
	return tld;
}

/*
 *  TLDNode_tldname returns the tld associated with the TLDNode
 */
char *TLDNode_tldname(TLDNode *node){
	return node->name;
}

/*
 *  TLDNode_count returns the value currently associated with that TLD
 */
long TLDNode_count(TLDNode *node){
	return node->count;
}