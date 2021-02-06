/*
	Name: driver.c
	Copyright: 
	Author: Allen
	Date: 02/02/21 19:11
	Description: Driver man for haribote.
*/

#include "bootpack.h"

struct dDevEntry{
	char *name;
	DEntry entry;
};

struct dDevEntry dDevs[2]={
	"FS",FSEntry, 
	"Ide HD",HDEntry
};

BOOL LDevs(struct MEMMAN *memman)
{
	int i;
	for(i=0;i<2;++i)
	{
		struct Dobject Devobj;
		Devobj.name = dDevs[i].name;
		Devobj.memman = memman;
		if((dDevs[i].entry)(&Devobj))
		{
			return FALSE;
		}
	}
	return TRUE;	
} 
