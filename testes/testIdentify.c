/**
** Teste da função cidentify
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"



int main(int argc, char *argv[]){
	int i;
	char groupName[] = "Nome: Filipe Joner | Cartao: 208840 \nNome: Jean Andrade | Cartao: \nNome: Rodrigo Dal Ri | Cartao: 244936";
	i = cidentify(groupName, sizeof(groupName));

	if(i==0){
		printf("\nPrograma encerrado corretamente!\n");
	}

	return 0;
}