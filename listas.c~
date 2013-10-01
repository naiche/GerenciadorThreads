#include<uthread.h>


ItemList* createList(void)
{
	return NULL;
}


TCB* removeFromList(ItemList *threadList, int tid)
{
	ItemLista *ant = NULL; //ponteiro auxiliar para a posição anterior
	ItemLista *ptaux = threadList; //ponteiro auxiliar para percorrer a lista

     	while (ptaux !=NULL && (ptaux->thTCB.tid != tid))      	//procura o elemento na lista
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
	ItemLista *ptaux = threadList; //ponteiro auxiliar para salvar o endereco do primeiro

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


ItemList* destroy(ItemList* threadList)
{
	ItemLista *ptaux; //ponteiro auxiliar para percorrer a lista
    	ItemLista *inicio; //ponteiro auxiliar para marcar o inicio da lista

    	inicio = threadlList;
    	while (threadList->proximo != inicio)
    	{
        	ptaux = threadList;
         	threadList = threadList->proximo;
         	free(ptaux);
    	}
    	free(threadList);
    	return NULL;
}
