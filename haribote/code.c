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
	code.code[code.next] = rand()%10000;
	code.next++;
	return code.code[code.next-1];	
}

BOOL VEcode(int vcode)
{
	int i;
	for(i=0;i<0xff;++i)
	{
		if(code.code[i]==vcode)
		{
			return TRUE;
		}
	}
	return FALSE;
}
