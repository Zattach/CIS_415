// Zach Domke
// zdomke
// CIS 415 Project 1
// This is my own work

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "p1fxns.h"
#include "queue.h"

#define MAX_BUF_SIZE 2048

int getQuantum(){
	char *p;
	int val = -1;

	if((p = getenv("USPS_QUANTUM_MSEC")) != NULL)
		val = p1atoi(p);
	return val;
}

int process(int quantum, int fd){
	int i = 0;
	int lines = 0;
	int status;
	const Queue *uq;
	char *buf = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
	char *program = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
	char *arguments = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
	
	if((uq = Queue_create(0L)) == NULL){
		p1perror(1, "queue problems\n");
		return 0;
	}

	while(p1getline(fd, buf, MAX_BUF_SIZE) != 0){
		if(!uq->enqueue(uq, buf)){
			p1perror(1, "enqueue problems");
			return 0;
		}
		lines++;
	}

	pid_t *pid = (int *)malloc(lines * sizeof(char));

	while(!uq->isEmpty(uq)){
		if(!uq->dequeue(uq, (void **)&buf)){
			p1perror(1, "dequeue problems");
			return 0;
		}
		pid[i] = fork();
		if(pid[i] == 0){
			p1getword(buf, 0, program);
			p1getword(buf, p1strchr(buf, ' '), arguments);
			execvp(&program[1], &arguments);
		}
		i++;
	}
	for(i = 0; i < lines; i++){
		wait(&status);
		usleep(quantum);
	}
	return 1;
}

int main(int argc, char *argv[]){
	int quantum;
	int fd;
	if(argc == 1){
		quantum = getQuantum();
		if(quantum == -1){
			p1perror(1, "no quantum\n");
			return 1;
		}
		fd = 0;
	}else if(argc == 2){
		char *word = "--quantum=";
		if(!p1strneq(word, argv[1], 10)){
			fd = open(argv[1], O_RDONLY);
			quantum = getQuantum();
			if(quantum == -1){
				p1perror(1, "no quantum\n");
				return 1;
			}
		}else{
			quantum = p1atoi(&argv[1][10]);
			if(quantum == 0){
				p1perror(1, "bad quantum\n");
				return 1;
			}
			fd = 0;
		}
	}else if(argc == 3){
		char *word = "--quantum=";
		quantum = getQuantum();
		if(!p1strneq(word, argv[1], 10)){
			p1perror(1, "bad quantum\n");
			return 1;
		}
		fd = open(argv[2], O_RDONLY);
	}else{
		p1perror(1, "bad arguments\n");
		return 1;
	}

	if(!process(quantum, fd)){
		p1perror(1, "bad process\n");
		return 1;
	}
	return 0;
}
