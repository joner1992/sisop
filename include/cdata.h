/*
 * cdata.h: arquivo de inclusão de uso apenas na geração da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida
 *
 */
#ifndef __cdata__
#define __cdata__
#include <ucontext.h>

#include "../include/support.h"
#include "../include/cthread.h"

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4

/* NÃO ALTERAR ESSA struct */
typedef struct s_TCB { 
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	int 		ticket;		// "bilhete" de loteria da thread, para uso do escalonador
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos) 
} TCB_t; 

typedef struct t_JOIN {
    int tid;
    int threadWaiting;
} BLOCK_join;

int createQueue(PFILA2 fila);
int generateTicket();
void runsThroughQueue(PFILA2 fila);
TCB_t *searchForBestTicket(PFILA2 fila, int loteryTicket);
int searchForTid(PFILA2 fila, int tid);

#endif
