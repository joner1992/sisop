/**
** Teste da função generateTicket
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"

void* ImprimeDez(void *arg) {

	printf("Works?");
	return (void*)NULL;
}


int main(int argc, char *argv[]){

	int valor = atoi(argv[0]);
	int i=0;
	for(i; i< 10; i++){
		int a = ccreate(ImprimeDez, (void*)&valor);
	}

	return 0;
}