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
	{"HAFS","hafs.sys"},
	{"IO","io.sys"},
	{"NET","net.sys"},
	{"SETM","setm.sys"},
	{"SETH","seth.sys"}
};

BOOL LDevs(struct MEMMAN *memman) {
	int i; 
	for(i=0; i<DR_NUM; ++i) {
		struct Dobject Devobj;
		Devobj.name = dDevs[i].name;
		Devobj.memman = memman;
		dDevs[i].Dobj = &Devobj;
		if(!(dDevs[i].entry)(&Devobj)) {
			return FALSE;
		}
	}
	/*for(i=0;i<DR_ENUM;i++)
	{
		struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
		struct Dobject Devobj;
		Devobj.name = etable[i].name;
		Devobj.memman = memman;
		etable[i].Dobj = &Devobj;

		//struct TASK *task;
		task = task_alloc();
		set_segmdesc(gdt + task.sel,);
		task->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
		task->tss.eip = (int) &FS_task;

		task_run(task, 4, 1);
	}*/

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
