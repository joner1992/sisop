#define _XOPEN_SOURCE 600


#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"

#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>
#include <stdlib.h>

#define SUCCESS 0
#define ERROR -1



#define	CRIACAO	0
#define	APTO	1
#define	EXEC	2
#define	BLOQ	3
#define	TERMINO	4

#define stackSize SIGSTKSZ


PFILA2 filaAptos = NULL;
int tid = 1; //Mantém tid global para enumerar threads

char groupName[] = "Nome: Filipe Joner | Cartao: 208840 \nNome: Jean Andrade | Cartao: \nNome: Rodrigo Dal Ri | Cartao: 244936";


int ccreate(void* (*start)(void*), void *arg) {

	TCB_t *newThread = (TCB_t*) malloc(sizeof(TCB_t));
	newThread->tid = tid;
	newThread->state = APTO;
	newThread->ticket = 1; // Valor dummie

	getcontext(&newThread->context);

	newThread->context.uc_link = 0;
	newThread->context.uc_stack.ss_sp = (char*) malloc(stackSize);

	if (newThread->context.uc_stack.ss_sp == NULL){
		return ERROR; // Erro ao alocar espaço para thread
	} 

	newThread->context.uc_stack.ss_size = stackSize;

	makecontext(&newThread->context, (void(*)(void))start, 1, arg);


	tid++;
	printf("Criou THREAD DE TID: %d\n", newThread->tid);


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