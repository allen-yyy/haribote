/*
	Name: FAT12.c 
	Copyright: 
	Author: Allen
	Date: 27/09/21 19:25
	Description: FAT12 FS
*/
#include "bootpack.h"
#include "FAT12.h"

int *fatg;

void FAT12_init()
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	fatg = (int *)memman_alloc_4k(memman,4*2880);
	FAT12_readfat(fatg, (unsigned char *) (ADR_DISKIMG + 0x000200)); 
	return;
}

void FAT12_readfat(int *fat, unsigned char *img)
{
	int i, j = 0;
	for (i = 0; i < 2880; i += 2) {
		fat[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
	return;
}
void FAT12_loadfile(int clustno, int size, char *buf, int *fat, char *img)
{
	int i;
	for (;;) {
		if (size <= 512) {
			for (i = 0; i < size; i++) {
				buf[i] = img[clustno * 512 + i];
			}
			break;
		}
		for (i = 0; i < 512; i++) {
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fat[clustno];
	}
	return;
}
struct FILEINFO *FAT12_search(char *name, struct FILEINFO *finfo, int max)
{
	int i, j;
	char s[12];
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++) {
		if (j >= 11) { return 0; /* 見つからなかった */ }
		if (name[i] == '.' && j <= 8) {
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				/* 小文字は大文字に直す */
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for (i = 0; i < max; ) {
		if (finfo->name[0] == 0x00) {
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i; /* ファイルが見つかった */
		}
next:
		i++;
	}
	return 0; /* 見つからなかった */
}
