/*
	Name: driver.c
	Copyright:
	Author: Allen
	Date: 02/02/21 19:11
	Description: Driver man for haribote.
*/

#include "bootpack.h"



struct exdrivertable {
	char *name;
	char *filename;
	//DEntry entry;
	struct Dobject *Dobj;
};

struct dDevEntry dDevs[DR_NUM] = {
	{"Ide HD",HDEntry},
	{"FS",FSEntry}
};

struct exdrivertable etable[DR_ENUM]= {
	/*{"HAFS","hafs.sys"},
	{"IO","io.sys"},
	{"NET","net.sys"},
	{"SETM","setm.sys"},
	{"SETH","seth.sys"}*/
	{"test","systest.sys"}
};



BOOL LDevs(struct MEMMAN *memman) {
	int i; 
	io_cli();
	for(i=0; i<DR_NUM; ++i) {
		struct Dobject Devobj;
		Devobj.name = dDevs[i].name;
		Devobj.memman = memman;
		dDevs[i].Dobj = &Devobj;
		io_sti();
		if(!(dDevs[i].entry)(&Devobj)) {
			return FALSE;
		}
	}
	io_sti();
	return TRUE;
}

void opensys(int *buf,int size);

BOOL load_external_device(int *fat,struct MEMMAN *memman)
{
	int i;
	int *buf;
	for(i=0;i<DR_ENUM;i++)
	{
		struct Dobject Devobj;
		memcpy(Devobj.name,etable[i].name,sizeof(etable[i].name)); 
		Devobj.memman = memman;
		etable[i].Dobj = &Devobj;
		struct FILEINFO *c = file_search(etable[i].filename, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
		buf = (int *)memman_alloc_4k(memman,c->size);
		file_loadfile(c->clustno,c->size,(char *)buf,fat,(char *) (ADR_DISKIMG + 0x003e00));
		struct TASK *task = task_alloc();
		task->tss.esp = memman_alloc_4k(memman, 64 * 1024)+64*1024-12;
		task->tss.eip = (int) &opensys;
		task->tss.es = 1*8;
		task->tss.cs = 1*8;
		task->tss.ss = 1*8;
		task->tss.ds = 1*8;
		task->tss.fs = 1*8;
		task->tss.gs = 1*8;
		*((int *) (task->tss.esp + 4)) = (int) buf;
		*((int *) (task->tss.esp + 8)) = (int) c->size;
		struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		putfonts8_asc(binfo->vram, binfo->scrnx, 0, 9, COL8_FFFFFF, "  t");
		task_run(task, 1, 1);
	}
	return TRUE;
}

struct Dobject *GetMyObj(char *name) {
	int i;
	for(i=0; i<DR_NUM; ++i) {
		if(strcmp(dDevs[i].name,name) == 0) {
			return dDevs[i].Dobj;
		}
	}
	return NULL;
}
void opensys(int *buf,int size)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK *task=task_now();
	int i=0;
	int segsiz = *((int *) (buf + 0x0000));
	int esp    = *((int *) (buf + 0x000c));
	int datsiz = *((int *) (buf + 0x0010));
	int dathrb = *((int *) (buf + 0x0014));
	char *q = (char *) memman_alloc_4k(memman, segsiz);
	task->ds_base = (int) q;
	set_segmdesc(gdt + task->sel / 8 + 1000+1000, size - 1, (int) buf, AR_CODE32_ER + 0x20);
	set_segmdesc(gdt + task->sel / 8 + 2000+1000, segsiz - 1,(int)  q, AR_DATA32_RW + 0x20);
	for (i = 0; i < datsiz; i++) {
		q[esp + i] = buf[dathrb + i];
	}
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "  t");
	start_sys(0x1b, (task->sel / 8 + 1000+1000)*8, esp, (task->sel / 8 + 2000+1000)*8, &(task->tss.esp0));
	task_sleep(task_now()); 
	return;
}
