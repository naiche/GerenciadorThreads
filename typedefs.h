/*
* typedefs.h: Arquivo com as definições de tipo.
*
* NÃO MODIFIQUE ESTE ARQUIVO.
*/
#ifndef __typedefs__
#define __typedefs__ 1

#include <ucontext.h>

typedef struct st_TCB TCB;
struct st_TCB
{
	int tid;
	ucontext_t* context;
	TCB *waiting_for_me;
};

#endif
