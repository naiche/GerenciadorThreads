/*
* typedefs.h: Arquivo com as definições de tipo.
*
* NÃO MODIFIQUE ESTE ARQUIVO.
*/
#ifndef __typedefs__
#define __typedefs__ 1

#include <ucontext.h>

/*DEFINES*/
#define STACKSIZE 10485760 //Stacksize for contexts
#define OK 0				//Ok return of the functions
#define ERROR -1			//Flag of error

/*TYPES*/
typedef struct st_TCB TCB;
struct st_TCB
{
	int tid;
	ucontext_t* context;
	TCB *waiting_for_me;
};

#endif
