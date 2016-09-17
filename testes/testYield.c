#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"

void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));

	cyield();
	puts("Voltou da Cyield()!");

	return;
}

int main(int argc, char *argv[]) {

	int	id0, id1, id2, id3, id4, id5, id6;
	int i;


	id0 = ccreate(func0, (void *)&i);
	id1 = ccreate(func0, (void *)&i);
	id2 = ccreate(func0, (void *)&i);
	id3 = ccreate(func0, (void *)&i);
	id4 = ccreate(func0, (void *)&i);

	cyield();
	puts("Voltou do primeiro Cyield() MAIN\n");
	cyield();
	puts("Voltou do segundo Cyield() MAIN\n");
	cyield();
	puts("Voltou do terceiro Cyield() MAIN\n");
	cyield();
	puts("Voltou do quato Cyield() MAIN\n");
	cyield();
	puts("Voltou do quinto Cyield() MAIN para finalizar o programa.\n");
	cyield();

	printf("Eu sou a main voltando para terminar o programa\n");
}
