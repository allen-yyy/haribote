/* ファイル関係 */

#include "bootpack.h"

void file_readfat(int *fat, unsigned char *img)
/* ディスクイメージ内のFATの圧縮をとく */
{
	int i, j = 0;
	for (i = 0; i < 2880; i += 2) {
		fat[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
	return;
}

void file_loadfile(int clustno, int size, char *buf, int *fat, char *img)
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

char *file_name(char *name,char *nowdir)
{
	char sum[0xfff];
	int i;
	for(i=0;nowdir[i]!='\0',i<0xfff;i++)
	{
		if(nowdir[i]=='\\'&&nowdir[i-1]=='\\') continue;
		sum[i]=nowdir[i];
	}
	for(;name[i]!='\0',i<0xfff;i++)
	{
		if(name[i]=='\\'&&name[i-1]=='\\') continue;
		sum[i]=name[i];
	}
	name[i+1]='\0';
	return sum;
}

struct FILEINFO *file_depth_search(char *name, struct FILEINFO *finfo)
{
	int n=224,end=0;
	name+=3;
	while(1)
	{
		int i;
		for(i=0;i<0xfff;i++)
		{
			if(name[i]=='\\') break;
			if(name[i]=='\0') end=1;
		}
		name[i]='\0';
		finfo=file_search(name,finfo,n);
		if(!finfo) break;
		if((finfo->type&0x10)==0&&end==0) break;
		else if(end==1) return finfo;
		name+=i+1;
	}
	return 0;
}

struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max)
{
	int i, j;
	char s[12];
	if(name[1]==':'&&name[2]=='\\')
	{
		return file_depth_search(name, finfo);
	}else{
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
			if ((finfo[i].type & 0x8) == 0) {
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
	}
	
	return 0; /* 見つからなかった */
}

char *file_loadfile2(int clustno, int *psize, int *fat)
{
	int size = *psize, size2;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char *buf, *buf2;
	buf = (char *) memman_alloc_4k(memman, size);
	file_loadfile(clustno, size, buf, fat, (char *) (ADR_DISKIMG + 0x003e00));
	if (size >= 17) {
		size2 = tek_getsize(buf);
		if (size2 > 0) {	/* tek圧縮がかかっていた */
			buf2 = (char *) memman_alloc_4k(memman, size2);
			tek_decomp(buf, buf2, size2);
			memman_free_4k(memman, (int) buf, size);
			buf = buf2;
			*psize = size2;
		}
	}
	return buf;
}
