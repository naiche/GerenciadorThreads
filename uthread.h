/*
* uthread.h: arquivo de inclusão com os prototypes das funções a serem
* implementadas na realização do trabalho.
*
* NÃO MODIFIQUE ESTE ARQUIVO.
*/
#include <ucontext.h>

typedef struct st_TCB TCB;
struct st_TCB
{
	int tid;
	ucontext_t* context;
	TCB *waiting_for_me;
};

#ifndef __uthread__
#define __uthread__ 1

int uth_init();
int uth_create(void * (*start_routine)(void*), void * arg);
int uth_yield(void);
int uth_wait(int thr);

#endif
