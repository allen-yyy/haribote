/*
	Name: driver.c
	Copyright: 
	Author: Allen
	Date: 02/02/21 19:11
	Description: Driver man for haribote.
*/

#include "bootpack.h"



struct dDevEntry dDevs[2]={
	{"FS",FSEntry},
	{"Ide HD",HDEntry}
};

BOOL LDevs(struct MEMMAN *memman)
{
	int i;
	for(i=0;i<DR_NUM;++i)
	{
		struct Dobject Devobj;
		Devobj.name = dDevs[i].name;
		Devobj.memman = memman;
		dDevs[i].Dobj = &Devobj;
		if((dDevs[i].entry)(&Devobj))
		{
			return FALSE;
		}
	}
	return TRUE;	
} 

struct Dobject *GetMyObj(char *name)
{
	int i;
	for(i=0;i<DR_NUM;++i)
	{
		if(strcmp(dDevs[i].name,name) == 0)
		{
			return dDevs[i].Dobj;
		}
	}
	return NULL;
}
