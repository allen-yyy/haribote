/*
	Name: code.c 
	Copyright: 
	Author: Allen
	Date: 11/02/21 19:26
	Description: code
*/

#include "bootpack.h"
int rand(void);

struct _codecore code;


void code_init()
{
	code.next=0;
	return; 
}
 

int code_alloc()
{
	int i;
	
	//for(i=0;i<885;i++){
	//	if(strcmp(codecore[i].name,name)==0)
	//	{
	//		codecore[i].codes[codecore[i].next] = rand();
	//		codecore[i].next++;
	//		return codecore[i].codes[codecore[i].next];
	//	}
	//} 
	
}
