/*
	Name: fs.c
	Copyright: 
	Author: Allen
	Date: 03/02/21 10:45
	Description: fs man
*/

#include "bootpack.h"

extern struct dev_callon *devcalls[10];
extern struct dDevEntry dDevs[DR_NUM];

void FS_task()
{
	struct MEMMAN *memman = MEMMAN_ADDR;
	struct Dobject *mydobj = GetMyObj("FS");
	struct Dobject *hddobj = dDevs[0].Dobj;
	int i=100;
	char s[20];
	struct MESSAGE message,umess;
	message.params = memman_alloc(memman,512);
	message.type = HD_OPEN;
	fs_send2hd(&message);
	//i = message_send(,&message);
	message_receive(ANY,&message);
	UINT sectors = *(UINT*)&message.params[60*2];
	struct fs_message *fmess;
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	sprintf(s,"taskrun %d",sectors*512/1024/1024);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	for(;;)
	{
		message_receive(ANY,&umess);
		switch(umess.type)
		{
			case FS_HDSIZE:
				message_send(task2pid(dDevs[1].Dobj->task),&umess);
				break;
			case FS_READ:
				fmess=umess.expar;
				message_send(devcalls[IDE_HD_CALLON]->pid,&message);
				message_receive(ANY,&message);
		}
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
	//*((int *) (task->tss.esp + 4)) = (int) Dobj->memman;
	task_run(task, 2 , 1);
	Dobj->task = task; 
	struct dev_callon fscallon;
	fscallon.id = FS_CALLON;
	fscallon.pid = task2pid(task);
	devcalls[FS_CALLON] = &fscallon;
	/*struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
		//sprintf(s,"taskrun %d",i);
		putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "123");**/
	return TRUE;
}
void fs_send2hd(struct MESSAGE *mess)
{
	struct hd2fs_message h2f;
	h2f.command = mess->type;
	h2f.isbuf = 1;
	mess->expar=&h2f;
	message_send(devcalls[IDE_HD_CALLON]->pid,mess);
	return;
} 
