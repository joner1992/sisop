/**
** Teste da função cidentify
**/

#include <stdio.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cthread.h"



int main(int argc, char *argv[]){
	char c[105] = "";
	int i = 1;
	i = cidentify(c, sizeof(c));
	if(i == -1){
		printf("size excedeu tamanho maximo para string. \n");
		
	}

	char d[104] = "";
	i = cidentify(d, sizeof(d));

	while(i<sizeof(d)){
		printf("%c", d[i]);
		i++;
	}	


	return 0;
}