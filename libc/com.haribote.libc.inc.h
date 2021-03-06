/* This is Haribote's libc
		name		date
		Allen		2020.10.24
		Allen		2020.10.24		printf putchar exit
		Allen		2020.10.25		malloc free
		Allen		2020.10.27		getchar 
		*/
int printf(char *format, ...);
int putchar(int c);
void exit(int status);
void *malloc(int size);
void free(void *p);
int getchar();
