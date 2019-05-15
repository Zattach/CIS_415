/* 
 * Zach Domke
 * zdomke
 * CIS 415 Project 2
 * This is my own work
 */

#include <pthread.h>
#include "networkdevice.h"
#include "BoundedBuffer.h"
#include "diagnostics.h"
#include "packetdescriptor.h"
#include "destination.h"
#include "pid.h"
#include "freepacketdescriptorstore.h"
#include "freepacketdescriptorstore__full.h"
#include "networkdevice.h"

/* any global variables required for use by your threads and your driver routines */
FreePacketDescriptorStore *freePackets;
BoundedBuffer *send_buff;
BoundedBuffer **recieve_buff;
BoundedBuffer *pool;

struct parms{
	NetworkDevice *nd;
	PacketDescriptor *pd;
};

/* definitions of functions required for your threads */
#define SIZE 3

void* send_thread(void *parameters){
	struct parms *p = (struct parms*)parameters;
	int result;
	while(1){
		send_buff->blockingRead(send_buff, p->pd);
		result = 0;
		for(int i=0; i<SIZE; i++){
			result = p->nd->sendPacket(p->nd, p->pd);
			if(result == 1){
				break;
			}
		}
		nonblocking_send_packet(p->pd);
	}
}

void* recieve_thread(void *parameters){
	struct parms *p = (struct parms*)parameters;
	PacketDescriptor *temp;
	initPD(temp);
	PID pid;
	freePackets->blockingGet(freePackets, &p->pd);
	initPD(p->pd);
	p->nd->registerPD(p->nd, p->pd);
	BoundedBuffer *temp_buff;
	while(1){
		p->nd->awaitIncomingPacket(p->nd);
		if(nonblocking_get_packet(temp) == 1){
			pid = getPID(p->pd);
			temp_buff = recieve_buff[pid];
			if(temp_buff->nonblockingWrite(temp_buff, p->pd) == 0){
				nonblocking_send_packet(p->pd);
			}
			p->pd = temp;
		}
		initPD(p->pd);
		p->nd->registerPD(p->nd, p->pd);
	}
}

void init_network_driver(NetworkDevice *nd, void *mem_start, 
						unsigned long mem_length, FreePacketDescriptorStore **fpds){
	/* 
	 * create Free Packet Descriptor Store using mem_start and mem_length
	 * create any bufers required by yout threads (send, recieve, and pool)
	 * create any threads you require for your implementation
	 * return the FPDS to the code that called you
	 */
	
	freePackets = FreePacketDescriptorStore_create(mem_start, mem_length);
	*fpds = freePackets;
	send_buff = BoundedBuffer_create(SIZE);
	*recieve_buff = BoundedBuffer_create(11);
	for(int i=0; i<11; i++){
		recieve_buff[i] = BoundedBuffer_create(SIZE);
	}
	pool = BoundedBuffer_create(SIZE);

	pthread_t send_t;
	pthread_t recieve_t;
	struct parms send_parms;
	struct parms recieve_parms;

	send_parms.nd = nd;
	initPD(send_parms.pd);
	pthread_create(&send_t, NULL, &send_thread, &send_parms);

	recieve_parms.nd = nd;
	initPD(recieve_parms.pd);
	pthread_create(&recieve_t, NULL, &recieve_thread, &recieve_parms);
}

void blocking_send_packet(PacketDescriptor *pd){
	/* 
	 * queue up packet descriptor for sending
	 * do not return until it has been successfully queued
	 */
	pool->blockingWrite(pool, pd);
}

int nonblocking_send_packet(PacketDescriptor *pd){
	/*
	 * if you are able to queue up packet descriptor immediately, do so and return 1
	 * otherwise, return 0
	 */
	if(pool->nonblockingWrite(pool, pd) == 0){
		if(freePackets->nonblockingPut(freePackets, pd) == 0){
			DIAGNOSTICS;
		}
	}
	return 1;
}

void blocking_get_packet(PacketDescriptor **pd, PID pid){
	/*
	 * wait until there is a packet for 'pid'
	 * return that packet descriptor to the calling application
	 */
	pool->blockingRead(pool, pd);
	setPID(*pd, pid);
}

int nonblocking_get_packet(PacketDescriptor **pd, PID pid){
	/*
	 * if there is currently a waiting packet for 'pid', return that packet
	 * to the calling application and return 1 for the value of the function
	 * otherwise, return 0 for the value of the function
	 */
	if(pool->nonblockingRead(pool, pd) == 0){
		if(freePackets->nonblockingGet(freePackets, pd) == 0){
			return 0;
		}
	}
	setPID(*pd, pid);
	return 1;
}
