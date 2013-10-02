#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include "uthread.h"
#include "tid.h"

int uth_init()
{
	set_tid(0);
	printf("%d should be 0 \n", get_tid());
	set_tid(3);
	printf("%d should be 3 \n",get_tid());
	printf("%d should be 4 \n",increment_tid());
	printf("%d should be 3 \n",decrement_tid());
	return 0;
}

int uth_create(void * (*start_routine)(void*), void * arg)
{
	return 0;
}

int uth_yield(void)
{
	return 0;
}

int uth_wait(int thr)
{
	return 0;
}
