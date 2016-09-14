#define _XOPEN_SOURCE 600


#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"

#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h> // Usada para gerar ticket com uint8_t

#define SUCCESS 0
#define ERROR -1

#define	CRIACAO	0
#define	APTO	1
#define	EXEC	2
#define	BLOQ	3
#define	TERMINO	4

#define stackSize SIGSTKSZ


char groupName[] = "Nome: Filipe Joner | Cartao: 208840 \nNome: Jean Andrade | Cartao: \nNome: Rodrigo Dal Ri | Cartao: 244936";


/************
* VARIÁVEIS GLOBAIS
************/

int tid = 1; //Mantém tid global para enumerar threads

PFILA2 filaAptos = NULL;
PFILA2 filaBloqueados = NULL;
int readyQueueCreated = 0;
int blockedQueueCreated = 0;

TCB_t *mainContext = NULL;
int mainContextCreated = 0;

/**************
* FUNÇÕES AUXILIARES
**************/
int generateTicket(){
	unsigned int random = Random2();
	uint8_t converter = random;
	int ticket = converter;
	return ticket;
}

int createMainContext(){


	return SUCCESS;
}

int createReadyQueue(){
	mainContextCreated = createMainContext();


	return SUCCESS;
}


int createBlockedQueue(){



	return SUCCESS;
}


/****************
*****************/
int ccreate(void* (*start)(void*), void *arg) {

	if(readyQueueCreated == 0){
		readyQueueCreated = createReadyQueue();
	}


	TCB_t *newThread = (TCB_t*) malloc(sizeof(TCB_t));
	newThread->tid = tid;
	newThread->state = APTO;
	newThread->ticket = generateTicket(); // Valor dummie

	getcontext(&newThread->context);

	newThread->context.uc_link = 0;
	newThread->context.uc_stack.ss_sp = (char*) malloc(stackSize);

	if (newThread->context.uc_stack.ss_sp == NULL){
		return ERROR; // Erro ao alocar espaço para thread
	} 

	newThread->context.uc_stack.ss_size = stackSize;

	makecontext(&newThread->context, (void(*)(void))start, 1, arg);


	tid++;
	printf("Criou THREAD DE TID: %d | TICKET: %d\n",newThread->tid, newThread->ticket);


	return newThread->tid;

}

int cyield(void) {

	return ERROR;
}

int cjoin(int tid) {

	return ERROR;

}

int csem_init(csem_t *sem, int count) {
	return ERROR;
}

int cwait(csem_t *sem) {
	return ERROR;
}

int csignal(csem_t *sem) {
	return ERROR;
}

int cidentify(char *name, int size) {
	int i = 0;
	if(*name <= 0 || size <= 0){
		return ERROR;
	} 

	while(i < size){
		printf("%c", *name);
		*name++;
		i++;
	}
	return SUCCESS;
}