/*
	Name: fs.c
	Copyright: 
	Author: Allen
	Date: 03/02/21 10:45
	Description: fs man
*/

#include "bootpack.h"
#include "FAT12.h"

extern struct dev_callon *devcalls[10];
extern struct dDevEntry dDevs[DR_NUM];
void fs_send2hd(struct MESSAGE *mess);
struct fs_partition{
	char letter;
	int is_mount;
	struct fs_struct fs_s;
}; 
struct fs_partition partitions[26]; 

/*void FS_task(void)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct MESSAGE message,umess;
	int fs=FST_NOWFS;
	{//send HD_OPEN
		message.params = (char *)memman_alloc(memman,512);
		message.type = HD_OPEN;
		fs_send2hd(&message);
		message_receive(ANY,&message);	
	}
	int i=0;
	for(i=0;i<26;i++)//init partitions 
	{
		partitions[i].letter='A'+i;
		partitions[i].is_mount=0;
	}
	UINT sectors = *(UINT*)&message.params[60*2];
	struct fs_message *fmess;
	for(;;)
	{
		message_receive(ANY,&umess);
		switch(umess.type)
		{
			case FS_INIT:
				fmess=(struct fs_message *)umess.expar;
				switch(fs)
				{
					case FST_FAT12:
						FAT12_init();
						partitions[fmess->letter-'A'].is_mount=1;
						break;
					case FST_HAFS1:
						partitions[fmess->letter-'A'].is_mount=1;
						break;
					default:
						io_cli(); /*BUG£¡*
						io_hlt();
				}
				break; 
				
			case FS_HDSIZE:
				//umess.Param=&sectors;
				//message_send(task2pid(dDevs[1].Dobj->task),&umess);
				break;
				
			///@fmess->name file name
			///@fmess->mode open mode
			case FS_OPEN:
				//char letter=fmess->name[0];
				//int handle=partitions[letter-'A'].fs_s.operations.open(fmess->name,fmess->mode);
				
				break;
			
			///@fmess->handle fs handle
			///@fmess->buf buf
			///@fmess->size read size
			case FS_READ:
				//fmess=umess.expar;
				//message_send(devcalls[IDE_HD_CALLON]->pid,&message);
				//message_receive(ANY,&message);
				
				break;
			case FS_WRITE:
				break; 
			default:
				break;
		}
	} 
	return;
} */

BOOL FSEntry(struct Dobject *Dobj)
{
	/*struct TASK *task;
	task = task_alloc();
	task->tss.esp = memman_alloc_4k(Dobj->memman, 64 * 1024) + 64 * 1024;
	task->tss.eip = (int) &FS_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	task->devflag=1;
	task_run(task, 2 , 1);
	Dobj->task = task; 
	struct dev_callon fscallon;
	fscallon.id = FS_CALLON;
	fscallon.pid = task2pid(task);
	devcalls[FS_CALLON] = &fscallon;*/
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


/* //debug1:hd size
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	sprintf(s,"taskrun %d",sectors*512/1024/1024);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
*/
