/*
	NAME: 			JOHNATHAN ZAMORA
	ID:					1201735331
	COURSE:			CSE 330 SP17
	ASSIGNMENT:	5
	FILE:				mem.h
 */

#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//CONSTANTS DEFINITIIONS
#define KB 	1000
#define MAX 1000
#define START 0
#define END 1000000
#define EMPTY -1

//STRUCTURE DEFINITIONS
typedef struct memory {
	struct block* bank; // points to the head of mem bank
} memory;

typedef struct block {
	int size; 					// in KB
	int processID;			// default to 0 for empty
	int start_address;
	int end_address;
	struct block* next;
} block;

//EXTERNALIZED STRUCTURES
extern memory* main_memory;
extern block* inputQueue;


// HELPER METHODS
block* allocateBlock(int id, int size, int start, int end, block* next) {
	block* new_block = malloc(sizeof(struct block));
	new_block->size = size;
	new_block->processID = id;
	new_block->start_address = start;
	new_block->end_address = end-1;
	new_block->next = next;

	return new_block;
}

block* partMem(block* new_block, block* toPart) {
	toPart->processID = -1;

	new_block->start_address = toPart->start_address;
	new_block->end_address = new_block->start_address + new_block->size*KB-1;

	if(new_block->size < toPart->size)
		new_block->next = allocateBlock(0, (toPart->size - new_block->size),
			(new_block->end_address + 1), (toPart->end_address + 1), toPart->next);

	return new_block;

}

void combineMem() {
	block* it;
	block* prev;

	it = main_memory->bank;
	prev = NULL;

	while(it != NULL && it->next != NULL) {
		if(it->processID == 0 && it->next->processID == 0) {
			if(prev == NULL){
				main_memory->bank = allocateBlock(0,(it->size + it->next->size),
						it->start_address, (it->end_address + 1), it->next->next);
				prev = main_memory->bank;
			}
			else {
				prev->next = allocateBlock(0,(it->size + it->next->size),
						it->start_address, (it->end_address + 1), it->next->next);
			}

			free(it->next);
			free(it);

			it = prev->next;
		}
		else{
			prev = it;
			it = it->next;
		}
	}
}

//MEMORY METHODS
void print() {
	block* it = main_memory->bank;
	printf("----------------------------------------------------------------------------------------\n");
	printf("Allocation Queue:\n");
	//allocated processes snapshot
	//printf("%05x",int var) to print in hex (0x12345)
	while(it != NULL) {
		if(it->processID == 0) {
			printf("Empty Block:	%dkb		Start Address:	0x%08x 	End Address:  0x%08x\n",
					it->size, it->start_address, it->end_address);
		}
		else{
			printf("Process %d:	%dkb		Start Address:	0x%08x 	End Address:  0x%08x\n",
					it->processID, it->size, it->start_address, it->end_address);
		}
		it = it->next;
	}
	printf("\nInput Queue:\n");
	//waiting processes
	it = inputQueue;
	if(it != NULL) {
		while(it != NULL){
			printf("Process %d:	%dkb\n", it->processID, it->size);
			it = it->next;
		}
	}
	else 
		printf("(empty)\n");
	printf("----------------------------------------------------------------------------------------\n\n");
}

int allocateMem(int processID, int memSize) {
	block* it;
	block* new_block;

	it = main_memory->bank;
	new_block = allocateBlock(processID,memSize,EMPTY,EMPTY,NULL);

	while(it != NULL) {
		if(memSize <= it->size && it->processID == 0) {
			//partion memory
			it = partMem(new_block, it);

			if(main_memory->bank->processID == -1) {
				free(main_memory->bank);
				main_memory->bank = it;
			}
			else {
				block* repoint = main_memory->bank;
				while(repoint->next->processID != -1){
					repoint = repoint->next;
				}
				free(repoint->next);
				repoint->next = it;
			}

			return 1;
		}
		else
			it = it->next;
	}
	//process did not fit into the memory
	it = inputQueue;

	if(it == NULL) {
		//nothing in the queue
		inputQueue = new_block;
	}
	else{
		//add to end of input queue
		while(it->next != NULL)
			it=it->next;

		it->next = new_block;
	}

	return 1;
}

void deleteMem(int processID) {
	if(processID > 0) {
		block* it = main_memory->bank;
		while(it->processID != processID){
			it = it->next;
		}
		it->processID = 0;

		combineMem();
		combineMem();

		if(inputQueue != NULL) {
			int count = 0;
			it = inputQueue;

			while(it != NULL) {
				count++;
				it = it->next;
			}
			while(count > 0) {
				it = inputQueue;
				inputQueue = it->next;
				allocateMem(it->processID, it->size);
				free(it);
				count--;
			}
		}
	}
}

#endif
