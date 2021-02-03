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
	"Ide HD",HDEntry,
	"FS",FSEntry
};

BOOL LDevs()
{
	int i;
	for(i=0;i<2;i++)
	{
		struct Dobject Devobj;
		Devobj.name = dDevs[i].name;
		if((dDevs[i].entry)(Devobj))
		{
			return FALSE;
		}
	}
	return TRUE;	
} 
