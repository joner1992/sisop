


CreateFila2(PFILA2 apto_fila);
CreateFila2(PFILA2 blocked_fila);



int dispatcher()
{
	

	
	
	
	
	
	
	return ;
}





void terminate_current_thread()
{
	TCB_t* thread = FirstFila2(PFILA2 apto_fila);
	thread -> state = 4;
	
	TCB_t* esperando = NULL;
	
	// percorrer fila de bloqueados, se achar alguem q esta esperando ela terminar passa para 'esperando'
	{
		FirstFila2(PFILA2 blocked_fila);
		while(blocked_fila->node != blocked_fila->last)	
			if(   == thread->tid){
				esperando = blocked_fila->node;
			}
		blocked_fila->it = blocked_fila->next;	
	}

	// alguem esta esperando ela terminar deve ser desbloqueado
	if (esperando != NULL)
	{
		//remover 'esperando' da fila de bloqueados
		DeleteAtIteratorFila2(PFILA2 blocked_fila);
		//alterar estado da thread
		thread->state = 1;
	}
	
	dispatcher();
}