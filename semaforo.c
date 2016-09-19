
FILA2 filaSem;

int csem_init(csem_t *sem, int count)
{
  if (uninitializedDependencies == 1) {
    uninitializedDependencies = initialize();
    if (uninitializedDependencies == ERROR) {
      return ERROR;
    }
  }
  
  createQueue(&filaSem);
  
  csem_t *sem = (csem_t*) malloc(sizeof(csem_t));
  sem->count = 1;
  sem->fila = *filaSem;
  
  return SUCCESS;
}

int cwait(csem_t *sem)
{
  if (uninitializedDependencies == 1) {
    uninitializedDependencies = initialize();
    if (uninitializedDependencies == ERROR) {
      return ERROR;
    }
  }

  if(sem->count<=0){
	   CPU->state = BLOQ;
	   AppendFila2(&filaSem, (void *) CPU);
  }
  
  else{
	//segue o baile
 }
   
  sem->count = (sem->count) - 1;

  return SUCCESS;
}

int csignal(csem_t *sem)
{
  if (uninitializedDependencies == 1) {
    uninitializedDependencies = initialize();
    if (uninitializedDependencies == ERROR) {
      return ERROR;
    }
  }
  
  TCB_t *tcb;
  
  FirstFila2(PFILA2 filaSem);
  
  tcb = (TCB_t*) GetAtIteratorFila2(filaSem);
  tcb->state = APTO;	
  
  DeleteAtIteratorFila2(PFILA2 filaSem);
    
  AppendFila2(PFILA2 filaAptos, void *tcb);
  
  sem->count = (sem->count) + 1;

  return SUCCESS;
}