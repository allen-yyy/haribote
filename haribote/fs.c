/*
	Name: fs.c
	Copyright: 
	Author: Allen
	Date: 03/02/21 10:45
	Description: fs man
*/

#include "bootpack.h"

BOOL FSEntry(struct Dobject Dobj)
{
	struct TASK *task = (struct TASK *)*((int *) 0x0f01);
	struct MESSAGE *message;
	message->type = HD_OPEN;
	message_send(task2pid(task),message);
	return FALSE;
}
