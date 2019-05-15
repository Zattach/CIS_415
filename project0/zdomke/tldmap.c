#include "date.h"
#include "tldmap.h"
#include "iterator.h"
#include <string.h>
#include <stdlib.h>

struct tldnode{
	char *name;
	long count;
	TLDNode *left;
	TLDNode *right;
};

typedef struct tlddata {
	long nitems;
	TLDNode *root;
} TLDData;

static void destroyRecursive(TLDNode *node){
	if(node != NULL){
		destroyRecursive(node->left);
		destroyRecursive(node->right);
		free(node);
	}
}

static void tld_destroy(const TLDMap *tld){
	TLDData *tldd = (TLDData *)tld->self;
	TLDNode *current = tldd->root;

	destroyRecursive(current);
	free(tldd);
	free(tld->self);
	free((void *)tld);
}

static int tld_insert(const TLDMap *tld, char *theTLD, long v){
	TLDData *tldd = (TLDData *)tld->self;
	TLDNode *current = tldd->root;
	if(!tld->lookup(tld, theTLD, &v)){
		TLDNode *curBack = NULL;
		while(current != NULL){
			curBack = current;
			if(strcmp(current->name, theTLD) < 0){
				current = current->left;
			}else{
				current = current->right;
			}
		}
		current = (TLDNode *)malloc(sizeof(TLDNode));
		if(current != NULL){
			current->name = theTLD;
			current->count = v;
			current->left = NULL;
			current->right = NULL;
			if(strcmp(curBack->name, theTLD) < 0){
				curBack->left = current;
			}else{
				curBack->right = current;	
			}
		}
		tldd->nitems++;
		return 1;
	}
	return 0;
}

static int tld_reassign(const TLDMap *tld, char *theTLD, long v){
	if(tld->lookup(tld, theTLD, &v)){
		TLDData *tldd = (TLDData *)tld->self;
		TLDNode *current = tldd->root;
		while(current->name != theTLD){
			if(strcmp(current->name, theTLD) < 0){
				current = current->left;
			}else{
				current = current->right;
			}
		}
		current->count = v;
		return 1;
	}
	return 0;
}

static int tld_lookup(const TLDMap *tld, char *theTLD, long *v){
	TLDData *tldd = (TLDData *)tld->self;
	TLDNode *current = tldd->root;
	while(current != NULL && strcmp(current->name, theTLD) != 0){
		if(strcmp(current->name, theTLD) < 0){
			current = current->left;
		}else{
			current = current->right;
		}
	}
	if(current != NULL){
		*v = current->count;
		return 1;
	}
	return 0;
}

static void inOrder(TLDNode *node, void **array, int *pos){
	if(node != NULL){
		inOrder(node->left, array, pos);
		array[*pos] = node;
		inOrder(node->right, array, pos);
	}
}

static const Iterator *tld_itCreate(const TLDMap *tld){
	TLDData *tldd = (TLDData *)tld->self;
    const Iterator *it = NULL;
    void **array = (void **)malloc(tldd->nitems * sizeof(void *));
    if (array != NULL) {
    	int *pos = 0;
    	inOrder(tldd->root, array, pos);
        it = Iterator_create(tldd->nitems, array);
        if (it == NULL)
            free(array);
    }
    return it;
}

static TLDMap template = {
	NULL, tld_destroy, tld_insert, tld_reassign, tld_lookup, tld_itCreate
};

char *TLDNode_tldname(TLDNode *node){
	return node->name;
}

long TLDNode_count(TLDNode *node){
	return node->count;
}

const TLDMap *TLDMap_create(void){
	TLDMap *tld = (TLDMap *)malloc(sizeof(TLDMap));

	if(tld != NULL){
		TLDData *tldd = (TLDData *)malloc(sizeof(TLDData));

		if(tldd != NULL){
			void **array = (void **)malloc(4 * sizeof(void *));
			if(array != NULL){
				tldd->nitems = 0;
				tldd->root = NULL;
				*tld = template;
				tld->self = tldd;
			}else{
				free(tldd);
				free(tld);
				tld = NULL;
			}
		}else{
			free(tld);
			tld = NULL;
		}
	}
	return tld;
}