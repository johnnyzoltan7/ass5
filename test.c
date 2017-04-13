/*
	NAME: 			JOHNATHAN ZAMORA
	ID:					1201735331
	COURSE:			CSE 330 SP17
	ASSIGNMENT:	5
	FILE:				test.c
*/

#include <stdlib.h>
#include "mem.h"

memory* main_memory;
block* inputQueue;

void test() {
	allocateMem(1,400);
	allocateMem(2,200);
	allocateMem(3,500);
	print();

	deleteMem(1);
	print();

	allocateMem(4,100);
	print();

	deleteMem(2);
	print();

	deleteMem(4);
	print();

	deleteMem(3);
	print();
}

int main() {
	//Initialize memory and input queue
	main_memory = malloc(sizeof(memory));
	main_memory->bank = allocateBlock(0,1000,START,END,NULL);
	inputQueue = NULL;

	test();

	return 1;
}
