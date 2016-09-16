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

ucontext_t contextDispatcher, contextTerminate; // Contextos para execução de funções de escalonador e de finalizador de threads

int tid = 1; //Mantém tid global para enumerar threads

FILA2 executando; //Fila criada para "simular" a CPU;
FILA2 filaAptos;
FILA2 filaBloqueados;
int uninitializedDependencies = 1;



/*************************************************************************************************************
***********************************************************************************
* ******************************     FUNÇÕES AUXILIARES E DE INICIALIZAÇÃO
***********************************************************************************
*************************************************************************************************************/
int createDispatcherContext() {
	getcontext(&contextDispatcher);
	printf("dispatch\n");
	contextDispatcher.uc_link = 0;
	contextDispatcher.uc_stack.ss_sp = (char*) malloc(stackSize);
	if (contextDispatcher.uc_stack.ss_sp == NULL) {
		return ERROR; // Erro ao alocar espaço para thread
	}

	contextDispatcher.uc_stack.ss_size = stackSize;
	makecontext(&contextDispatcher, (void(*)(void))dispatch, 2, &filaAptos, &executando);

	return SUCCESS;
}

int createTerminaterContext() {
	getcontext(&contextTerminate);
	contextTerminate.uc_link = 0;
	contextTerminate.uc_stack.ss_sp = (char*) malloc(stackSize);
	if (contextTerminate.uc_stack.ss_sp == NULL) {
		return ERROR; // Erro ao alocar espaço para thread
	}

	contextTerminate.uc_stack.ss_size = stackSize;
	makecontext(&contextTerminate, (void(*)(void))terminate, 4, &contextDispatcher, &filaBloqueados, &filaAptos, &executando);
	return SUCCESS;
}


int createMainContext() {
	//gera Contexto da main
	TCB_t *mainThread = (TCB_t*) malloc(sizeof(TCB_t));
	mainThread->tid = MAINTID;
	mainThread->state = EXEC;
	mainThread->ticket = generateTicket(); // Valor dummie

	getcontext(&mainThread->context);

	if (mainThread == NULL) {
		return ERROR;
	}
	else {
		printf("Criou Main Context.\n");
		printf("Criou THREAD DE TID: %d | TICKET: %d\n", mainThread->tid, mainThread->ticket);

		int addedToExecutingQueue;
		addedToExecutingQueue = AppendFila2(&executando, (void *) mainThread);
		if (addedToExecutingQueue == SUCCESS) {
			printf("Adicionou na fila de EXECUCAO!\n");
			return SUCCESS;
		}
		else {
			return ERROR;
		}

	}
}

int createBlockedQueue() {
	//Inicializa fila de bloqueados
	return createQueue(&filaBloqueados);

}


int createReadyQueue() {
	//Inicializa fila de aptos
	return createQueue(&filaAptos);
}




int initialize() {
	// Criar MainContext
	// Criar fila de bloqueados
	// Criar fila de aptos
	// Criar threads de dispatcher e terminate
	// Fila de semáforos irá ser criada apenas quando for necessária

	int dispatcherContextCreated = 0;
	int terminateContextCreated = 0;

	int blockedQueueinitilized = 0;
	int readyQueueinitilized = 0;
	int mainContextCreated = 0;



	blockedQueueinitilized = createBlockedQueue();
	printf("Chegou no initialize!\n");
	readyQueueinitilized = createReadyQueue();
	mainContextCreated = createMainContext();
	dispatcherContextCreated = createDispatcherContext();
	terminateContextCreated = createTerminaterContext();

	if (mainContextCreated == ERROR ||
	        blockedQueueinitilized == ERROR || readyQueueinitilized == ERROR
	        || dispatcherContextCreated == ERROR || terminateContextCreated == ERROR ) {
		return ERROR;
	}
	else {
		return SUCCESS;
	}

}


/*************************************************************************************************************
***********************************************************************************
* ******************************     FUNÇÕES QUE DEVEM SER NECESSARIAMENTE IMPLEMENTADAS
***********************************************************************************
*************************************************************************************************************/
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

	newThread->context.uc_stack.ss_sp = (char*) malloc(stackSize);

	if (newThread->context.uc_stack.ss_sp == NULL) {
		return ERROR; // Erro ao alocar espaço para thread
	}
	newThread->context.uc_stack.ss_size = stackSize;
	newThread->context.uc_link = &contextTerminate;

	makecontext(&newThread->context, (void(*)(void))start, 1, arg);


	tid++;
	printf("Criou THREAD DE TID: %d | TICKET: %d\n", newThread->tid, newThread->ticket);

	int addedToReadyQueue;
	addedToReadyQueue = AppendFila2(&filaAptos, (void *) newThread);
	if (addedToReadyQueue == SUCCESS) {
		printf("Adicionou na fila de aptos!\n");
		return newThread->tid;
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

	//VERIFICA DE tid existe
	// SAIR DA FILA DE EXECUÇAO
	// ENTRAR NA FILA DE BLOQUEADOS
	// SALVAR CONTEXTO ATUAL
	// SETAR CONTEXTO PARA DISPATCHER

	int processTidExists;
	processTidExists = searchForTid(&filaAptos, tid);
	if (processTidExists == ERROR) {
		return ERROR;
	}

	int first;
	first = FirstFila2(&executando);
	if (first == SUCCESS) {
		void *node;
		node = GetAtIteratorFila2(&executando);
		TCB_t *tcb = (TCB_t*) malloc(sizeof(TCB_t));
		tcb = (TCB_t*) node;
		int deleted;
		deleted = DeleteAtIteratorFila2(&executando);
		if(deleted == SUCCESS){
			tcb->state = BLOQ;
			int addedToBlockedQueue;
			addedToBlockedQueue = AppendFila2(&filaBloqueados, (void *) tcb);
			if (addedToBlockedQueue == SUCCESS) {
				swapcontext(&tcb->context, &contextDispatcher);
			}
		}
		else{
			printf("Deu ruin na fila exectudando no cjoin");
			return ERROR;
		}
	}
	return SUCCESS;
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



/*************************************************************************************************************
***********************************************************************************
* ******************************     FUNÇÕES DE TESTESS
***********************************************************************************
*************************************************************************************************************/