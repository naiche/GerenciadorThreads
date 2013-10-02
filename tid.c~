#include "tid.h"

int* get_tid_object()
{
	static int tid = 0;
	return &tid;
}

int get_tid()
{
	return *(get_tid_object());
}

void set_tid(int value)
{
	*(get_tid_object()) = value;
}

int increment_tid()
{
	return ++*(get_tid_object());
}

int decrement_tid()
{
	int* tid_obj = get_tid_object();
	if(*(tid_obj)>0)
		--*(tid_obj);
	return *(tid_obj);
}
