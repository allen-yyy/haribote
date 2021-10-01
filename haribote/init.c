
#include "bootpack.h"

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
