/*
* tid.h:
*
* NÃO MODIFIQUE ESTE ARQUIVO.
*/
#ifndef __tid__
#define __tid__ 1

/*Gets the tid value
	return: tid value
*/
int get_tid();

/*Set the tid value
	int value = value to be setted
	return: nothing
*/
void set_tid(int value);

/*Increment the tid value
	return: the new tid value, after the increment
*/
int increment_tid();

/*Decrement the tid value. If tid is 0, do nothing
	return: the new tid value 
*/
int decrement_tid();

#endif
