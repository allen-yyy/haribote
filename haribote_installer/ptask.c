#include "bootpack.h"

int *ptask_table[MAX_TASK];
int ptask_nums;

struct PTASK *ptask_alloc(struct TASK *task)
{
	struct PTASK *ptask;
	int i;
	for(i=0;i<MAX_PTASK;i++)
	{
		if(task->ptasks[i].flag==0)
		{
			ptask=&(task->ptasks[i]);
			ptask->flag=1;
			ptask->id=i;
			ptask->tss.eflags = 0x00000202; /* IF = 1; */
			ptask->tss.eax = 0; /* とりあえず0にしておくことにする */
			ptask->tss.ecx = 0;
			ptask->tss.edx = 0;
			ptask->tss.ebx = 0;
			ptask->tss.ebp = 0;
			ptask->tss.esi = 0;
			ptask->tss.edi = 0;
			ptask->tss.es = 0;
			ptask->tss.ds = 0;
			ptask->tss.fs = 0;
			ptask->tss.gs = 0;
			ptask->tss.iomap = 0x40000000;
			ptask->tss.ss0 = 0;
			
		}
	}
	return ptask;
}

void ptask_add(struct PTASK *ptask)
{
	ptask->flag+=2;//bit 1 set
	return;
}

void ptask_remove(struct PTASK *ptask)
{
	ptask->flag-=2;//bit 1 unset
	return;
}

void ptask_init(struct TASK *task)
{
	task->ptasks[0].flag=1;
	task->ptasks[0].id=0;
	task->ptasks[0].pid=task->pid;
	task->ptasks[0].priority=task->priority;
	task->ptasks[0].tss_s.cs=task->sel;
	task->ptasks[0].tss_s.eip=task->tss.eip;
	task->ptasks[0].tss_s.esp=task->tss.esp;
	task->ptasks[0].tss_s.esp0=0;
	
	task->ptasknum=1;
	return;
}

void ptask_switch(struct TASK *task)
{
	return;
}
