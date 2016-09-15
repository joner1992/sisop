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


/******************
* FUNÇÕES AUXILIARES
*******************/
int createQueue(PFILA2 fila) {
	//Inicializa fila de bloqueados
	int initializedQueue;
	initializedQueue = CreateFila2(fila);

	if (initializedQueue == ERROR) {
		return ERROR;
	}
	else {
		return SUCCESS;
	}
}


int generateTicket() {
	unsigned int random = Random2();
	uint8_t converter = random;
	int ticket = converter;
	return ticket;
}


void runsThroughQueue(PFILA2 fila){
	int work;
	work = FirstFila2(fila);
	if(work==0){
		void *tcb;
		tcb = GetAtIteratorFila2(fila);
		TCB_t *teste = (TCB_t*) malloc(sizeof(TCB_t));
		teste = (TCB_t*) tcb;
		printf("%d\n", teste->tid);
		int work2 = 0;
		while(work2 == 0){
			work2 = NextFila2(fila);
			tcb = GetAtIteratorFila2(fila);
			if(tcb == NULL){
				return;
			}
			teste = (TCB_t*) tcb;
			printf("%d\n", teste->tid);
		}

	}

}
