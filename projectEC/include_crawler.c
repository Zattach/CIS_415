/* 
 * Zach Domke
 * zdomke
 * CIS 415 ProjectEC
 * This is my own work
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "tslinkedlist.h"
#include "tshashmap.h"

typedef struct parsed_file{
	char *name;
	char *type;
} Parsed_File;


FILE *open_file(char *afile, const TSLinkedList *directories){
	long size = directories->size(directories);
	char the_file[100] = "\0";
	// char **temp = (char **)(directories->toArray(directories, &size));
	FILE *fp;
	for(long i=0; i < size; i++){
		directories->get(directories, i, (void **)&the_file);
		strcpy(the_file, (char *)afile);
		if((fp = fopen(the_file, "r"))){
			return fp;
		}
	}
	return NULL;
}

Parsed_File *parse_file(char *file){
	Parsed_File *temp = (Parsed_File *)malloc(sizeof(Parsed_File));
	temp->name = strtok(file, ".");
	temp->type = strtok(NULL, ".");
	return temp;
}

void print_dependencies(const TSHashMap *the_table, const TSLinkedList *printed, const TSLinkedList *to_process){
	int in;
	char *fn = NULL;
	char *namell = NULL;
	char *nameprinted = NULL;
	TSLinkedList *ll;
	to_process->removeFirst(to_process, (void **)&fn);
	while(fn != NULL){
		the_table->get(the_table, fn, (void *)&ll);
		for(long i=0; i < ll->size(ll); i++){
			in = 0;
			ll->get(ll, i, (void **)&namell);
			for(long j=0; j < printed->size(printed); j++){
				printed->get(printed, j, (void **)&nameprinted);
				if(strcmp(namell, nameprinted) == 0){
					in = 1;
					break;
				}
			}
			if(!in){
				printf(" %s", namell);
				printed->add(printed, namell);
				to_process->addLast(to_process, namell);
			}
		}
		to_process->removeFirst(to_process, (void **)&fn);
	}
}

void process(char *afile, const TSLinkedList *deps, const TSHashMap *the_table, const TSLinkedList *work_queue, const TSLinkedList *directories){
	FILE *f = open_file(afile, directories);
	if(f == NULL){
		printf("Unable to open file: %s\n", afile);
		return;
	}

	char *line = NULL;
	char *l = NULL;
	char include[8] = "#include";
	size_t len = 0;
	ssize_t nread;
	while((nread = getline(&line, &len, f)) != -1){
		if(strncmp(include, line, 8) == 0){
			if((l = strtok(line, "\""))){
				l = strtok(NULL, "\"");
				deps->addLast(deps, l);
				if(the_table->containsKey(the_table, l) == 0){
					the_table->putUnique(the_table, l, (void *)TSLinkedList_create());
					work_queue->addLast(work_queue, l);
				}
			}
		}
	}
	fclose(f);
}

int main(int argc, char *argv[]){
	const TSLinkedList *directories = TSLinkedList_create();
	directories->add(directories, "./");
	int fstart = 0;
	for(int i=1; i < argc; i++){
		if(argv[i][0] == '-' && argv[i][1] == 'I'){
			if((strcmp(&argv[i][strlen(argv[i])-1], "/")))
				directories->add(directories, argv[i]);
			else
				directories->add(directories, strcat(argv[i], "/"));
		} else {
			fstart = i;
			break;
		}
	}

	char *cpath = getenv("CPATH");
	if(cpath != NULL){
		for(char *dirs = strtok(cpath, ":"); dirs; dirs = strtok(NULL, ":")){
			if(strcmp(&dirs[strlen(dirs) - 1], "/"))
				directories->add(directories, dirs);
			else
				directories->add(directories, strcat(dirs, "/"));
		}
	}

	const TSLinkedList *work_queue = TSLinkedList_create();
	const TSLinkedList *deps = NULL;
	const TSHashMap *the_table = TSHashMap_create(0, 0);
	Parsed_File *L = (Parsed_File *)malloc(sizeof(Parsed_File));
	char *obj = (char *)malloc(sizeof(char) * 32);
	char *filename = (char *)malloc(sizeof(char) * 32);
	for(int i=fstart; i < argc; i++){
		strcpy(filename, argv[i]);
		L = parse_file(argv[i]);
		if(strncmp(L->type, "c", 1) != 0 && strncmp(L->type, "l", 1) != 0 && strncmp(L->type, "y", 1) != 0){
			printf("Illegal arguments: %s must end in .c, .l, or .y\n", L->name);
			return -1;
		}
		strcat(L->name, ".o");
		strcpy(obj, L->name);
		deps = TSLinkedList_create();
		deps->add(deps, filename);
		the_table->putUnique(the_table, obj, (void *)deps);
		work_queue->addLast(work_queue, filename);
		deps = TSLinkedList_create();
		the_table->putUnique(the_table, filename, (void *)deps);
	}

	char *afile = NULL;
	work_queue->removeFirst(work_queue, (void **)&afile);
	while(afile != NULL){
		the_table->get(the_table, afile, (void **)&deps);
		process(afile, deps, the_table, work_queue, directories);
		the_table->putUnique(the_table, afile, (void *)deps);
		work_queue->removeFirst(work_queue, (void **)&afile);
	}

	const TSLinkedList *printed = TSLinkedList_create();
	const TSLinkedList *to_process = NULL;
	for(int i=fstart; i < argc; i++){
		L = parse_file(argv[i]);
		obj = strcat(L->name, ".o");
		printf("%s:", obj);
		printed->add(printed, obj);
		to_process = TSLinkedList_create();
		to_process->addLast(to_process, obj);
		print_dependencies(the_table, printed, to_process);
		printf("\n");
	}
	return 0;
}
