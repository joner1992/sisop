/**
** Teste da função generateTicket
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"

void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));

	cyield();
	puts("TERIA QUE PRINTAR AQUI, MAS ESCALONADOR CHAMOU ANTES!");

	return;
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
	cyield();
	puts("TERIA QUE PRINTAR AQUI, MAS ESCALONADOR CHAMOU ANTES!");
}

int main(int argc, char *argv[]) {

	int	id0, id1, id2, id3, id4, id5, id6;
	int i;


	id0 = ccreate(func0, (void *)&i);
	id1 = ccreate(func1, (void *)&i);
	id2 = ccreate(func1, (void *)&i);
	id3 = ccreate(func1, (void *)&i);
	id4 = ccreate(func1, (void *)&i);
	id5 = ccreate(func1, (void *)&i);
	id6 = ccreate(func1, (void *)&i);

	printf("Eu sou a main após a criação de ID0 e ID1\n");

	i = cjoin(id0);
	i = cjoin(id1);
	i = cjoin(id2);
	i = cjoin(id3);
	i = cjoin(id4);
	i = cjoin(id5);
	i = cjoin(id6);

	printf("Eu sou a main voltando para terminar o programa\n");
}
