/*
	Name: fs.c
	Copyright: 
	Author: Allen
	Date: 03/02/21 10:45
	Description: fs man
*/

#include "bootpack.h"

void FS_task(struct MEMMAN *memman)
{
	struct Dobject *mydobj = GetMyObj("FS");
	struct Dobject *hddobj = GetMyObj("Ide HD");
	int i=0;
	struct MESSAGE *message;
	message->type = HD_OPEN;
	message_send(task2pid(hddobj->task),message);
	message_receive(ANY,message);
	for(;;)
	{
		i++;
		*((int *) 0x0ef0) = i;
		io_hlt();
		//struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		//boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
		//sprintf(s,"taskrun %d",i);
		//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "113234546");
	 } 
	return;
} 

BOOL FSEntry(struct Dobject *Dobj)
{
	struct TASK *task;
	task = task_alloc();
	task->tss.esp = memman_alloc_4k(Dobj->memman, 64 * 1024) + 64 * 1024;
	task->tss.eip = (int) &FS_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	task_run(task, 4, 1);
	Dobj->task = task; 
	/*struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
		//sprintf(s,"taskrun %d",i);
		putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "123");*/
	return TRUE;
}