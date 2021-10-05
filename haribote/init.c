
#include "bootpack.h"
void run_init_functions();
void initAcpi();
void init_all_device()
{
	run_init_functions();
	initAcpi();
	return;
}

void run_init_functions()
{
	return;
}
