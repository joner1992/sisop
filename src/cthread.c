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
#define MAINTID 0

/************
* VARIÁVEIS GLOBAIS
************/

int tid = 1; //Mantém tid global para enumerar threads


FILA2 filaAptos;
FILA2 filaBloqueados;
int uninitializedDependencies = 1;


/**************
* FUNÇÕES AUXILIARES
**************/
int generateTicket() {
	unsigned int random = Random2();
	uint8_t converter = random;
	int ticket = converter;
	return ticket;
}


int createMainContext() {
	//gera Contexto da main
	TCB_t *mainThread = (TCB_t*) malloc(sizeof(TCB_t));
	mainThread->tid = MAINTID;
	mainThread->state = APTO;
	mainThread->ticket = generateTicket(); // Valor dummie

	getcontext(&mainThread->context);

	if (mainThread == NULL) {
		return ERROR;
	}
	else {
		printf("Criou Main Context.\n");
		printf("Criou THREAD DE TID: %d | TICKET: %d\n", mainThread->tid, mainThread->ticket);

		int addedToReadyQueue;
		addedToReadyQueue = AppendFila2(&filaAptos, (void *) mainThread);
		if (addedToReadyQueue == SUCCESS) {
			printf("Adicionou na fila de aptos!\n");
			return SUCCESS;
		}
		else {
			return ERROR;
		}

	}
}

int createBlockedQueue() {
	//Inicializa fila de bloqueados
	int initializedBlockedQueue;
	initializedBlockedQueue = CreateFila2(&filaBloqueados);

	if (initializedBlockedQueue == ERROR) {
		return ERROR;
	}
	else {
		printf("Criou Fila de Bloqueados.\n");
		return SUCCESS;
	}
}


int createReadyQueue() {
	//Inicializa fila de aptos
	int initializedReadyQueue;
	initializedReadyQueue = CreateFila2(&filaAptos);

	if (initializedReadyQueue == ERROR) {
		return ERROR;
	}
	else {
		printf("Criou Fila de Aptos.\n");
		return SUCCESS;
	}

}




int initialize() {
	// Criar MainContext
	// Criar fila de bloqueados
	// Criar fila de aptos
	// Fila de semáforos irá ser criada apenas quando for necessária

	int readyQueueinitilized = 0;
	int blockedQueueinitilized = 0;
	int mainContextCreated = 0;

	blockedQueueinitilized = createBlockedQueue();
	printf("Checou no initialize!\n");
	readyQueueinitilized = createReadyQueue();
	mainContextCreated = createMainContext();

	if (mainContextCreated == ERROR || blockedQueueinitilized == ERROR || readyQueueinitilized == ERROR) {
		return ERROR;
	}
	else {
		return SUCCESS;
	}

}



void runsThroughReadyQueue(){
	int work;
	work = FirstFila2(&filaAptos);
	if(work==0){
		void *tcb;
		tcb = GetAtIteratorFila2(&filaAptos);
		TCB_t *teste = (TCB_t*) malloc(sizeof(TCB_t));
		teste = (TCB_t*) tcb;
		printf("%d\n", teste->tid);
		int work2 = 0;
		while(work2 == 0){
			work2 = NextFila2(&filaAptos);
			tcb = GetAtIteratorFila2(&filaAptos);
			if(tcb == NULL){
				return;
			}
			teste = (TCB_t*) tcb;
			printf("%d\n", teste->tid);
		}

	}

}


/****************
*****************/
int ccreate(void* (*start)(void*), void *arg) {

	if (uninitializedDependencies == 1) {
		uninitializedDependencies = initialize();
		if (uninitializedDependencies == ERROR) {
			return ERROR;
		}
	}


	TCB_t *newThread = (TCB_t*) malloc(sizeof(TCB_t));
	newThread->tid = tid;
	newThread->state = APTO;
	newThread->ticket = generateTicket(); // Valor dummie

	getcontext(&newThread->context);

	newThread->context.uc_link = 0;
	newThread->context.uc_stack.ss_sp = (char*) malloc(stackSize);

	if (newThread->context.uc_stack.ss_sp == NULL) {
		return ERROR; // Erro ao alocar espaço para thread
	}

	newThread->context.uc_stack.ss_size = stackSize;

	makecontext(&newThread->context, (void(*)(void))start, 1, arg);


	tid++;
	printf("Criou THREAD DE TID: %d | TICKET: %d\n", newThread->tid, newThread->ticket);

	int addedToReadyQueue;
	addedToReadyQueue = AppendFila2(&filaAptos, (void *) newThread);
	if (addedToReadyQueue == SUCCESS) {
		printf("Adicionou na fila de aptos!\n");
		return newThread->tid;;
	}
	else {
		return ERROR;
	}


}

int cyield(void) {
	if (uninitializedDependencies == 1) {
		uninitializedDependencies = initialize();
		if (uninitializedDependencies == ERROR) {
			return ERROR;
		}
	}

	return ERROR;
}

int cjoin(int tid) {
	if (uninitializedDependencies == 1) {
		uninitializedDependencies = initialize();
		if (uninitializedDependencies == ERROR) {
			return ERROR;
		}
	}

	return ERROR;

}

int csem_init(csem_t *sem, int count) {
	if (uninitializedDependencies == 1) {
		uninitializedDependencies = initialize();
		if (uninitializedDependencies == ERROR) {
			return ERROR;
		}
	}

	return ERROR;
}

int cwait(csem_t *sem) {
	if (uninitializedDependencies == 1) {
		uninitializedDependencies = initialize();
		if (uninitializedDependencies == ERROR) {
			return ERROR;
		}
	}


	return ERROR;
}

int csignal(csem_t *sem) {
	if (uninitializedDependencies == 1) {
		uninitializedDependencies = initialize();
		if (uninitializedDependencies == ERROR) {
			return ERROR;
		}
	}

	return ERROR;
}

int cidentify(char *name, int size) {
	char groupName[] = "Nome: Filipe Joner Cartao: 208840\nNome: Jean Andrade Cartao: 252846\nNome: Rodrigo Dal Ri Cartao: 244936";
	int realSize = sizeof(groupName);
	int i = 0;
	if (size <= 0 || size > realSize) {
		return ERROR;
	}

	while (i < size) {
		*name = groupName[i];
		name++;
		i++;
	}
	return SUCCESS;
}



/***************
* FUNÇÕES DE TESTE
****************/
