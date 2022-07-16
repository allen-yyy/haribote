/* This is Haribote's libc
		name		date
		Allen		2020.10.19
		Allen		2020.10.24		printf putchar exit
		Allen		2020.10.25		malloc free
		Allen		2020.10.27		getchar 
		*/ 
/*
		printf l23
		putchar l36
		exit l42
		malloc l47
		free l58
		getchar l71

*/
#include "../apilib.h"
#include <stdio.h>
#include <stdarg.h> 
#include "com.haribote.libc.inc.h" 


int printf(char *format, ...)
{
	va_list ap;
	char s[1000];
	int i;
	
	va_start(ap,format);
	i = vsprintf(s,format,ap);
	api_putstr0(s);
	va_end(ap);
	return i;
}

int putchar(int c)
{
	api_putchar(c);
	return c;
}

void exit(int status)
{
	api_end();
	return; 
}

void *malloc(int size)
{
	char *p = api_malloc(size + 16);
	if(p != 0)
	{
		*((int *) p) = size;
		p += 16;
	}
	return p;	
}

void free(void *p)
{
	char *q = p;
	int size;
	if(q!=0)
	{
		q-=16;
		size=*((int*) q);
		api_free(q,size+16);
	}
	return;
}

int getchar()
{
	int c;
	c = api_getkey(1);
	return c;
}











