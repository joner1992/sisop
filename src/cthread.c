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

#define CRIACAO 0
#define APTO  1
#define EXEC  2
#define BLOQ  3
#define TERMINO 4

#define stackSize SIGSTKSZ
#define MAINTID 0

/************
* VARIÁVEIS GLOBAIS
************/

// Contextos para execução de funções de escalonador e de finalizador de threads
ucontext_t contextDispatcher, contextTerminator; 

int tid = 1; //Mantém tid global para enumerar threads

TCB_t *CPU; //Fila criada para "simular" a CPU;
TCB_t mainThread;
FILA2 filaAptos;
FILA2 filaBloqueados;
int uninitializedDependencies = 1;



/*******************************************************************************
********************************************************************************
* ******************************     FUNÇÕES ESCALONADOR
********************************************************************************
*******************************************************************************/
int clearCPU()
{
	if(CPU->tid != MAINTID){
	  free(CPU->context.uc_stack.ss_sp);
	  free(CPU);
	  CPU = NULL;	
	}
  return SUCCESS;

}

void terminate()
{
  //VERIFICAR FILA DE BLOQUEADOS -> SE ALGUM ESTIVER BLOQUEADO,
  //																DESBLOQUEIA O PROCESSO;
  // VERIFICAR FILA DE SEMÁFORO -> CWAIT() / CSIGNAL()
  // RETIRA PROCESSO DE ESTADO EXECUTANDO
  
    printf("ENTROU PARA TERMINATE\n");
    unjoinProcesses(&filaBloqueados, &filaAptos, CPU->tid);
    clearCPU();
    setcontext(&contextDispatcher);

}



void dispatch()
{
  // Gera bilhete de loteria  ++
  // Percorre fila para achar os que mais se aproximam ++
  // Seleciona a thread a ser executada ++
  // retira ela da fila de aptos ++
  // faz swap para o contexto selecionado ++
  TCB_t *nextProcess;
  int loteryTicket = generateTicket();

  nextProcess = searchForBestTicket(&filaAptos, loteryTicket);
  printf("TICKET: %d | TICKET ESCOLHIDO: %d | TID escolhido: %d\n", 
         loteryTicket, nextProcess->ticket, nextProcess->tid);

  nextProcess->state = EXEC;
  CPU = nextProcess;
  setcontext(&CPU->context);
}


/*******************************************************************************
********************************************************************************
* *************  FUNÇÕES AUXILIARES E DE INICIALIZAÇÃO
********************************************************************************
*******************************************************************************/

int createDispatcherContext()
{
  getcontext(&contextDispatcher);
  printf("dispatch\n");
  contextDispatcher.uc_link = 0;
  contextDispatcher.uc_stack.ss_sp = (char*) malloc(stackSize);
  if (contextDispatcher.uc_stack.ss_sp == NULL) {
    return ERROR; 
  }
  contextDispatcher.uc_stack.ss_size = stackSize;
	makecontext(&contextDispatcher, (void(*)(void))dispatch,0); 
	return SUCCESS;

}

int createTerminatorContext()
{
  getcontext(&contextTerminator);
  printf("dispatch\n");
  contextTerminator.uc_link = 0;
  contextTerminator.uc_stack.ss_sp = (char*) malloc(stackSize);
  if (contextTerminator.uc_stack.ss_sp == NULL) {
    return ERROR; 
  } 
  contextTerminator.uc_stack.ss_size = stackSize;
	makecontext(&contextTerminator, (void(*)(void))terminate,0);
  return SUCCESS;

}

int createBlockedQueue()
{
  //Inicializa fila de bloqueados
  return createQueue(&filaBloqueados);

}

int createMainContext() {
	//gera Contexto da main
	mainThread.tid = MAINTID;
	mainThread.state = EXEC;
	mainThread.ticket = generateTicket(); // Valor dummie

	getcontext(&mainThread.context);

	printf("Criou Main Context.\n");
	printf("Criou THREAD DE TID: %d | TICKET: %d\n", 
	mainThread.tid, mainThread.ticket);

	CPU = &mainThread;
	if(CPU){
		printf("Adicionou a CPU!\n");
		return SUCCESS;
	}
	else {
		return ERROR;
	}

}


int createReadyQueue()
{
  //Inicializa fila de aptos
  return createQueue(&filaAptos);
}

int initialize()
{
  // Criar MainContext
  // Criar fila de bloqueados
  // Criar fila de aptos
  // Criar threads de dispatcher e terminate
  // Fila de semáforos irá ser criada apenas quando for necessária

  int dispatcherContextCreated;
  int terminateContextCreated;

  int blockedQueueinitilized;
  int readyQueueinitilized;
  int mainContextCreated;



  blockedQueueinitilized = createBlockedQueue();
  printf("Chegou no initialize!\n");
  readyQueueinitilized = createReadyQueue();
  mainContextCreated = createMainContext();
  dispatcherContextCreated = createDispatcherContext();
  terminateContextCreated = createTerminatorContext();

  if (mainContextCreated == ERROR || 
      blockedQueueinitilized == ERROR || 
      readyQueueinitilized == ERROR || 
      dispatcherContextCreated == ERROR || 
      terminateContextCreated == ERROR) {
    return ERROR;
  }
  else {
    return SUCCESS;
  }

}





/*******************************************************************************
********************************************************************************
* **************  FUNÇÕES QUE DEVEM SER NECESSARIAMENTE IMPLEMENTADAS
********************************************************************************
*******************************************************************************/
int ccreate(void* (*start)(void*), void *arg)
{

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
  newThread->context.uc_link = &contextTerminator;

  makecontext(&newThread->context, (void(*)(void))start, 1, arg);


  tid++;
  printf("Criou THREAD DE TID: %d | TICKET: %d\n", 
         newThread->tid, newThread->ticket);

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

int cjoin(int tid)
{
	// VERIFICA DE tid existe na fila de Aptos ou Bloqueados ++
  // SAIR DA FILA DE EXECUÇAO
  // ENTRAR NA FILA DE BLOQUEADOS
  // SALVAR CONTEXTO ATUAL
  // SETAR CONTEXTO PARA DISPATCHER
  if (searchForTid(&filaAptos, tid) == ERROR && 
  	  searchForTid(&filaBloqueados, tid) == ERROR) {
    return ERROR;
  }else{
	  CPU->state = BLOQ;
	  if(AppendFila2(&filaBloqueados, (void *) CPU) == SUCCESS){
		  puts("ENTROU PARA BLOQUEADOS");
	  }
  }
  swapcontext(&CPU->context, &contextDispatcher);
  return SUCCESS;

}

int cyield(void)
{
  if (uninitializedDependencies == 1) {
    uninitializedDependencies = initialize();
    if (uninitializedDependencies == ERROR) {
      return ERROR;
    }
  }

   
  return ERROR;
}

int csem_init(csem_t *sem, int count)
{
  if (uninitializedDependencies == 1) {
    uninitializedDependencies = initialize();
    if (uninitializedDependencies == ERROR) {
      return ERROR;
    }
  }

  return ERROR;
}

int cwait(csem_t *sem)
{
  if (uninitializedDependencies == 1) {
    uninitializedDependencies = initialize();
    if (uninitializedDependencies == ERROR) {
      return ERROR;
    }
  }


  return ERROR;
}

int csignal(csem_t *sem)
{
  if (uninitializedDependencies == 1) {
    uninitializedDependencies = initialize();
    if (uninitializedDependencies == ERROR) {
      return ERROR;
    }
  }

  return ERROR;
}

int cidentify(char *name, int size)
{
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



/*******************************************************************************
********************************************************************************
* ******************************     FUNÇÕES DE TESTESS
********************************************************************************
*******************************************************************************/