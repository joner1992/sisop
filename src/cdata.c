


CreateFila2(PFILA2 apto_fila);

CreateFila2(PFILA2 blocked_fila);



int dispatcher()
{
	

	
	
	
	
	
	
	return ;
}





void terminate_current_thread()
{
	TCB_t* thread = FirstFila2(PFILA2 apto_fila);
	thread -> state = '4';
	
	TCB_t* esperando = NULL;
	
	// percorrer lista de bloqueados 
	int i;
	for (i=0; i<    ; ++i)
	{
		if (   ->  == thread->tid)
		{
			esperando =   ;
			break;
		}
	}

	 // alguem esta esperando ela terminar deve ser desbloqueado
	if (esperando != NULL)
	{
        
		unblock(esperando);//essa função existe???
	}
	
	dispatcher();
}