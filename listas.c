//#include "uthread.h"
#include "listas.h"
#define NULL 0


ItemList* createList(void)
{
	return NULL;
}


int insertList(ItemList *threadList, TCB *aThread)
{
	ItemList *novo; //novo elemento
	int length = 1;
	ItemList *ptaux = threadList; //ponteiro auxiliar para percorrer a lista     	
	
	novo = (ItemList*) malloc(sizeof(ItemList));
	novo->thTCB = aThread;
	novo->proximo = NULL;

	if (threadList == NULL)
	{
		threadList = novo;
	}
	else
	{
		while (ptaux->proximo != NULL)      	//procura o elemento na lista
     		{
			ptaux = ptaux->proximo;
			length++;
     		}
		ptaux->proximo = novo;
	}	
	return length;
}


TCB* removeFromList(ItemList *threadList, int tid)
{
	ItemList *ant = NULL; //ponteiro auxiliar para a posição anterior
	ItemList *ptaux = threadList; //ponteiro auxiliar para percorrer a lista

     	while (ptaux != NULL && (ptaux->thTCB->tid != tid))      	//procura o elemento na lista
     	{
		ant = ptaux;
		ptaux = ptaux->proximo;
     	}

     	if (ptaux == NULL)  //se nao achou
		return NULL;

    	if (ant == NULL) 
	{
		threadList = ptaux->proximo;  //Remove o primeiro elemento
    		return ptaux;	
	}
	else
	{
		ant->proximo = ptaux->proximo;  //Remove o elemento desejado
    		return ptaux;	
	}   
}


TCB* removeFirstList(ItemList *threadList)
{
	ItemList *ptaux = threadList; //ponteiro auxiliar para salvar o endereco do primeiro

     	if (ptaux == NULL)  //se nao achou
	{
		return NULL;
	}
    	else
	{ 
		threadList = threadList->proximo;  //Remove o primeiro elemento
    		return ptaux;	
	}
}


ItemList* destroy(ItemList *threadList)
{
	ItemList *ptaux; //ponteiro auxiliar para percorrer a lista
    	ItemList *inicio; //ponteiro auxiliar para marcar o inicio da lista

    	inicio = threadList;
    	while (threadList->proximo != inicio)
    	{
        	ptaux = threadList;
         	threadList = threadList->proximo;
         	free(ptaux);
    	}
    	free(threadList);
    	return NULL;
}
