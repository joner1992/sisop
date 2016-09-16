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




TCB_t *tcb;
TCB_t *unjoin;
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

void dispatch(PFILA2 filaAptos, PFILA2 executando) {
	// Gera bilhete de loteria  ++
	// Percorre fila para achar os que mais se aproximam ++
	// Seleciona a thread a ser executada ++
	// retira ela da fila de aptos ++
	// faz swap para o contexto selecionado ++
	TCB_t *nextProcess = (TCB_t*) malloc(sizeof(TCB_t));
	int loteryTicket = generateTicket();

	nextProcess = searchForBestTicket(filaAptos, loteryTicket);
	printf("TICKET: %d | TICKET ESCOLHIDO: %d | TID escolhido: %d\n", loteryTicket, nextProcess->ticket, nextProcess->tid);

	nextProcess->state = EXEC;
	int executingVerifier;
	executingVerifier = AppendFila2(executando, (void *)nextProcess);
	if (executingVerifier == SUCCESS) {
		setcontext(&nextProcess->context);
	}

}

void terminate(ucontext_t contextMain, PFILA2 filaBloqueados, PFILA2 filaAptos, PFILA2 executando) {
	//VERIFICAR FILA DE BLOQUEADOS -> SE ALGUM ESTIVER BLOQUEADO,DESBLOQUEIA O PROCESSO;
	//VERIFICAR FILA DE SEMÁFORO -> CWAIT() / CSIGNAL()
	// RETIRA PROCESSO DE ESTADO EXECUTANDO
	int first;
	puts("Terminate process");
	first = LastFila2(executando);
	if (first == SUCCESS) {
		printf("ENTROU NA FILA DE EXECUTANDO\n");
		void *node;
		node = GetAtIteratorFila2(executando);
		TCB_t *tcb = (TCB_t*) malloc(sizeof(TCB_t));
		tcb = (TCB_t*) node;

		unjoinProcesses(filaBloqueados, filaAptos, tcb->tid);
		DeleteAtIteratorFila2(executando);
		tcb->state = TERMINO;

		setcontext(&contextMain);

	}
	else{
		puts("ERRO NA FILA EXECUTANDO");
	}
}


TCB_t *searchForBestTicket(PFILA2 fila, int loteryTicket) {
	// Se o primeiro for igual, retira ele da fila e retorna o tcb
	//	se não, compara valor de tickets e coloca em min_ticket e aux_tid (para encontrá-lo na próxima iteração)
	// se não
	//	Procura na fila e compara com ticket
	//		se igual retira da fila
	//		se não compara valor de tickets e coloca em min_ticket e aux_tid (para encontrá-lo na próxima iteração)

	int bestTID;
	int bestValue;

	int first;
	first = FirstFila2(fila);

	if (first == SUCCESS) {
		void *node;
		node = GetAtIteratorFila2(fila);
		tcb = (TCB_t*) node;

		if (tcb->ticket == loteryTicket) {
			DeleteAtIteratorFila2(fila);
			return tcb;
		}
		else {
			bestValue = abs(tcb->ticket - loteryTicket);
			bestTID = tcb->tid;

			int iterator = 0;
			while (iterator == SUCCESS) {
				iterator = NextFila2(fila);
				node = GetAtIteratorFila2(fila);
				int auxValue;
				if (node == NULL) {
					first = FirstFila2(fila);
					node = GetAtIteratorFila2(fila);
					tcb = (TCB_t*) node;
					if (tcb->tid == bestTID) {
						DeleteAtIteratorFila2(fila);
						return tcb;
					}
					else {
						iterator = 0;
						while (iterator == SUCCESS) {
							iterator = NextFila2(fila);
							node = GetAtIteratorFila2(fila);
							tcb = node;
							if (tcb->tid == bestTID) {
								DeleteAtIteratorFila2(fila);
								return tcb;
							}
						}
					}
				}
				else {
					if (tcb->ticket == loteryTicket) {
						DeleteAtIteratorFila2(fila);
						return tcb;
					}
					else {
						auxValue = abs(tcb->ticket - loteryTicket);
						if (auxValue <= bestValue) {
							bestValue = auxValue;
							bestTID = tcb->tid;

						}
					}

				}

			}
			return NULL;
		}
	}
	else {
		return NULL;
	}

}

int searchForTid(PFILA2 fila, int tid) {
	int first;
	first = FirstFila2(fila);
	if (first == SUCCESS) {
		void *tcb;
		tcb = GetAtIteratorFila2(fila);
		TCB_t *wanted = (TCB_t*) malloc(sizeof(TCB_t));
		wanted = (TCB_t*) tcb;
		if (wanted->tid == tid) {
			printf("Achou tid para join\n");
			return SUCCESS;
		}
		else {
			int iterator = 0;
			while (iterator == 0) {
				iterator = NextFila2(fila);
				tcb = GetAtIteratorFila2(fila);
				if (tcb == NULL) {
					printf("DEU RUIM tid para join\n");
					return ERROR;
				}
				else {
					wanted = (TCB_t*) tcb;
					if (wanted->tid == tid) {
						printf("Achou tid para join\n");
						return SUCCESS;
					}
				}
			}
			printf("DEU RUIM tid para join\n");
			return ERROR;
		}
	}
	else {
		printf("DEU RUIM tid para join\n");
		return ERROR;
	}

}

void unjoinProcesses(PFILA2 filaBloqueados, PFILA2 filaAptos, int tidThreadTerminated) {
	int first;
	first = FirstFila2(filaBloqueados);
	if (first == SUCCESS) {
		void *node;
		node = GetAtIteratorFila2(filaBloqueados);
		unjoin = (TCB_t*) node;
		if (unjoin->tid == tidThreadTerminated) {
			DeleteAtIteratorFila2(filaBloqueados);
			AppendFila2(filaAptos, (void *) unjoin);
		}
		else {
			int iterator = 0;
			while (iterator == 0) {
				iterator = NextFila2(filaBloqueados);
				node = GetAtIteratorFila2(filaBloqueados);
				if (node == NULL) {
					return;
				}
				else {
					unjoin = (TCB_t*) node;
					if (unjoin->tid == tidThreadTerminated) {
						DeleteAtIteratorFila2(filaBloqueados);
						AppendFila2(filaAptos, (void *) unjoin);
					}
				}
			}
			return;
		}
	}
	else
		return;


}


void runsThroughQueue(PFILA2 fila) {
	int work;
	work = FirstFila2(fila);
	if (work == 0) {
		void *tcb;
		tcb = GetAtIteratorFila2(fila);
		TCB_t *teste = (TCB_t*) malloc(sizeof(TCB_t));
		teste = (TCB_t*) tcb;
		printf("%d\n", teste->tid);
		int work2 = 0;
		while (work2 == 0) {
			work2 = NextFila2(fila);
			tcb = GetAtIteratorFila2(fila);
			if (tcb == NULL) {
				return;
			}
			teste = (TCB_t*) tcb;
			printf("%d\n", teste->tid);
		}

	}
}
