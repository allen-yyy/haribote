/* コンソール関係 */

#include "bootpack.h"
#include <stdio.h>
#include <string.h>

extern struct dDevEntry dDevs[DR_NUM];
int abs(int);
char now_dir[100000];
int printk_line=0;

int printk(char *format, ...)
{
	va_list ap;
	char s[1000];
	int i;
	
	va_start(ap,format);
	i = vsprintf(s,format,ap);
	error_print(s);
	va_end(ap);
	return i;
}

void error_print(char *s)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	int x=0,y=printk_line;
	extern char hankaku[4096];
	for(;*s!=0;s++)
	{
		if(*s==0x0a) {y+=16;continue;}
		if(*s==0x0d) {x=0;continue;}
		boxfill8(binfo->vram, binfo->scrnx, COL8_000000, x, y, x+16, y+16);
		putfont8(binfo->vram, binfo->scrnx, x, y, COL8_FFFFFF, hankaku + *s * 16);
		x+=10;
	}
	printk_line=y;
	return;
}
#define IS_LEAP_YEAR(year) ((year % 4 == 0&&year % 100 !=0) || (year % 400==0))

void readrtc(unsigned char *t)
{
    char err;
    static unsigned char adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
    static unsigned char max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
    int i;
    for (;;) { /* ﾕi､ﾟﾞz､ﾟ､ｬｳﾉｹｦ､ｹ､�ﾞ､ﾇﾀR､�ｵ､ｹ */
        err = 0;
        for (i = 0; i < 7; i++) {
            io_out8(0x70, adr[i]);
            t[i] = io_in8(0x71);
        }
        for (i = 0; i < 7; i++) {
            io_out8(0x70, adr[i]);
            if (t[i] != io_in8(0x71) || (t[i] & 0x0f) > 9 || t[i] > max[i]) {
                err = 1;
            }
        }
        if (err == 0) {
            return;
        }
    }
}

void printtime(struct CONSOLE *cons)
{
    unsigned char s[24], t[7];
    readrtc(t);
    sprintf(s, "%02X%02X.%02X.%02X %02X:%02X:%02X\n", t[6], t[5], t[4], t[3], t[2], t[1], t[0]);
    cons_putstr0(cons,s);
}
#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

/* interestingly, we assume leap-years */
static int month[12] = {
	0,
	DAY*(31),
	DAY*(31+29),
	DAY*(31+29+31),
	DAY*(31+29+31+30),
	DAY*(31+29+31+30+31),
	DAY*(31+29+31+30+31+30),
	DAY*(31+29+31+30+31+30+31),
	DAY*(31+29+31+30+31+30+31+31),
	DAY*(31+29+31+30+31+30+31+31+30),
	DAY*(31+29+31+30+31+30+31+31+30+31),
	DAY*(31+29+31+30+31+30+31+31+30+31+30)
};

long kernel_mktime(struct TIME *tm)
{
	long res;
	int year;

	year = tm->year - 1970;
	res = YEAR*year + DAY*((year+1)/4);
	res += month[tm->moon];
	if (tm->moon>1 && ((year+2)%4))
		res -= DAY;
	res += DAY*(tm->day-1);
	res += HOUR*tm->hour;
	res += MINUTE*tm->min;
	res += tm->sec;
	return res;
}
unsigned int time()
{
	struct TIME *tm=time2TIME();
	return kernel_mktime(tm);
} 
void print_identify_info(short* hdinfo,struct CONSOLE *cons)
{
	int i, k;
	char s[64];
	char b[800]; 

	struct iden_info_ascii {
		int idx;
		int len;
		char * desc;
	} iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII */
		     {27, 40, "HD Model"} /* Model number in ASCII */ };

	for (k = 0; k < sizeof(iinfo)/sizeof(iinfo[0]); k++) {
		char * p = (char*)&hdinfo[iinfo[k].idx];
		for (i = 0; i < iinfo[k].len/2; i++) {
			s[i*2+1] = *p++;
			s[i*2] = *p++;
		}
		s[i*2] = 0;
		sprintf(b,"%s: %s\n", iinfo[k].desc, s);
		cons_putstr0(cons,s);
	}

	int capabilities = hdinfo[49];
	sprintf(b,"LBA supported: %s\n",
	       (capabilities & 0x0200) ? "Yes" : "No");
	cons_putstr0(cons,s);
	int cmd_set_supported = hdinfo[83];
	sprintf(b,"LBA48 supported: %s\n",
	       (cmd_set_supported & 0x0400) ? "Yes" : "No");
	cons_putstr0(cons,s);
	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	sprintf(b,"HD size: %dMB\n", sectors * 512 / 1000000);
	cons_putstr0(cons,s);
}

struct TIME *time2TIME()
{
	struct TIME time;
	unsigned char *t;
	readrtc(t);
	time.year = (t[6]<<100)+t[5];
	time.moon = t[4];
	time.day = t[3];
	time.min = t[0];
	time.sec = t[1];
	time.hour = t[2];
	return &time;
}

void hdinfo(struct CONSOLE *cons)
{
	struct MESSAGE message;
	char s[30];
	message.type = 1;
	message_send(task2pid(dDevs[1].Dobj->task),&message);
	message_receive(ANY,&message);
	sprintf(s,"%dMB\n\n",*message.params);
	cons_putstr0(cons,s);
	return;
} 

void console_task(struct SHEET *sheet, int memtotal)
{
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	struct FILEHANDLE fhandle[8];
	char cmdline[30];
	unsigned char *nihongo = (char *) *((int *) 0x0fe8);

	cons.sht = sheet;
	cons.cur_x =  8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	task->cons = &cons;
	task->cmdline = cmdline;

	if (cons.sht != 0) {
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->fifo, 1);
		timer_settime(cons.timer, 50);
	}
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
	for (i = 0; i < 8; i++) {
		fhandle[i].buf = 0;	/* 未使用マーク */
	}
	task->fhandle = fhandle;
	task->fat = fat;
	if (nihongo[4096] != 0xff) {	/* 日本語フォントファイルを読み込めたか？ */
		task->langmode = 1;
	} else {
		task->langmode = 0;
	}
	task->langbyte1 = 0;
	now_dir[0]='C';now_dir[1]=':';now_dir[2]='/';now_dir[3]='\0';now_dir[4]='\0';
	
	int dirlen=4;
	/* プロンプト表示 */
	cons_putstr0(&cons, now_dir);
	cons_putchar(&cons, '>', 1);
	int l=0,num=0;
	for (;;) {
		io_cli();
		if(num<0) num=0;
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons.sht != 0) { /* カーソル用タイマ */
				if (i != 0) {
					timer_init(cons.timer, &task->fifo, 0); /* 次は0を */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_FFFFFF;
					}
				} else {
					timer_init(cons.timer, &task->fifo, 1); /* 次は1を */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_000000;
					}
				}
				timer_settime(cons.timer, 50);
			}
			if (i == 2) {	/* カーソルON */
				cons.cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* カーソルOFF */
				if (cons.sht != 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, COL8_000000,
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				cons.cur_c = -1;
			}
			if (i == 4) {	/* コンソールの「×」ボタンクリック */
				cmd_exit(&cons, fat);
			}
			if (256 <= i && i <= 511) { /* キーボードデータ（タスクA経由） */
				if (i == 8 + 256) {
					/* バックスペース */
					if (cons.cur_x > 8+dirlen*8) {
						/* カーソルをスペースで消してから、カーソルを1つ戻す */
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
						num--;
					}else if(l>0&&cons.cur_x > 8)
					{
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
						num--;
					}else if(l>0&&cons.cur_x <= 8)
					{
						cons_putchar(&cons, ' ', 0);
						cons.cur_x = 240;
						l-=240;
						cons.cur_y -= 16;
						num--;
					}
				} else if (i == 10 + 256) {
					/* Enter */
					/* カーソルをスペースで消してから改行する */
					cons_putchar(&cons, ' ', 0);
					cmdline[num] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal, now_dir);	/* コマンド実行 */
					if (cons.sht == 0) {
						cmd_exit(&cons, fat);
					}
					io_cli();
					//cmdline[0]='\0';
					//struct FIFO32 *tmp=(struct FIFO *)0x0fec;
					//task_run(tmp->task,-1,0);
					/* プロンプト表示 */
					cons_putstr0(&cons, now_dir);
					cons_putchar(&cons, '>', 1);
					num=0;
					l=0;
				} else {
					/* 一般文字 */
					if (cons.cur_x < 240) {
						/* 一文字表示してから、カーソルを1つ進める */
						cmdline[num] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}else{
						cons_putchar(&cons, ' ', 1);
						//cons_putchar(&cons, '\n', 1);
						cmdline[num] = i - 256;
						l+=cons.cur_x;
						cons_putchar(&cons, ' ', 0);
						cons.cur_x=8;
						cons_putchar(&cons, i - 256, 1);
					}
					num++;
				}
			}
			io_sti();
			/* カーソル再表示 */
			if (cons.sht != 0) {
				if (cons.cur_c >= 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, cons.cur_c, 
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				sheet_refresh(cons.sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
			}
		}
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move)
{
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {	/* タブ */
		for (;;) {
			if (cons->sht != 0) {
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			}
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0) {
				break;	/* 32で割り切れたらbreak */
			}
		}
	} else if (s[0] == 0x0a) {	/* 改行 */
		cons_newline(cons);
	} else if (s[0] == 0x0d) {	/* 復帰 */
		/* とりあえずなにもしない */
	} else {	/* 普通の文字 */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		}
		if (move != 0) {
			/* moveが0のときはカーソルを進めない */
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	struct TASK *task = task_now();
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16; /* 次の行へ */
	} else {
		/* スクロール */
		if (sheet != 0) {
			for (y = 28; y < 28 + 112; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
				}
			}
			for (y = 28 + 112; y < 28 + 128; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = COL8_000000;
				}
			}
			sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
		}
	}
	cons->cur_x = 8;
	if (task->langmode == 1 && task->langbyte1 != 0) {
		cons->cur_x = 16;
	}
	return;
}

void cons_putstr0(struct CONSOLE *cons, char *s)
{
	for (; *s != 0; s++) {
		cons_putchar(cons, *s, 1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l)
{
	int i;
	for (i = 0; i < l; i++) {
		cons_putchar(cons, s[i], 1);
	}
	return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, int memtotal, char *now_dir)
{
	if (strcmp(cmdline, "mem") == 0 && cons->sht != 0) {
		cmd_mem(cons, memtotal);
	} else if (strcmp(cmdline, "cls") == 0 && cons->sht != 0) {
		cmd_cls(cons);
	} else if (strcmp(cmdline, "dir") == 0 && cons->sht != 0) {
		cmd_dir(cons);
	} else if (strcmp(cmdline, "exit") == 0) {
		cmd_exit(cons, fat);
	} else if (strncmp(cmdline, "start ", 6) == 0) {
		cmd_start(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "ncst ", 5) == 0) {
		cmd_ncst(cons, cmdline, memtotal);
	} else if (strncmp(cmdline, "langmode ", 9) == 0) {
		cmd_langmode(cons, cmdline);
	} else if (strcmp(cmdline, "time") == 0){
		printtime(cons);
	} else if (strcmp(cmdline, "shutdown") == 0){
		acpiPowerOff();
	} else if (strcmp(cmdline, "version") == 0){
		ver(cons);
	} else if (strcmp(cmdline, "ver") == 0){
		ver(cons);
	} else if (strcmp(cmdline, "hdinfo") == 0){
		hdinfo(cons);
	} else if (strcmp(cmdline, "ps") == 0){
		ps(cons);
	} else if (strncmp(cmdline, "cd ",2) == 0){
		cons_putstr0(cons, "cd : Error!Not ready!\n\n");
		cd(cons, cmdline, now_dir); 
	} else if (strcmp(cmdline, "pcidebug") == 0){
		pci_test(cons);
	} else if (cmdline[0] != 0) {
			if (cmd_app(cons, fat, cmdline, now_dir) == 0) {
				/* コマンドではなく、アプリでもなく、さらに空行でもない */
				cons_putstr0(cons, "Bad command.\n\n");
			}
		}
	return;
}


void ps(struct CONSOLE *cons)
{
	char s[31];
	int i,j=0;
	extern struct pid_t pid;
	sprintf(s,"PID\tname\n");
	cons_putstr0(cons,s);
	sprintf(s,"%d\tHaribote kernel\n",0);
	cons_putstr0(cons,s);
	for(i=10000;i<=pid.next;i++)
	{
		if(!pid2task(i)->devflag)
		{
			if(i==10001) sprintf(s,"%d\tCPU idle\n",i-10000),j++;
			else
			sprintf(s,"%d\t%s\n",i-10000,pid2task(i)->name),j++;
			cons_putstr0(cons,s);
		}
	}
	sprintf(s,"%dtasks run\n",j+1);
	cons_putstr0(cons,s);
	return;
}

void cd(struct CONSOLE *cons)
{
	return;
}

void ver(struct CONSOLE *cons)
{
	char s[61];
	cons_putstr0(cons, "Haribote version 0.2B\n\n");
	return;
}

void cmd_mem(struct CONSOLE *cons, int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char s[60];
	sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 128; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[30];
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			if ((finfo[i].type & 0x18) == 0) {
				sprintf(s, "filename.ext   %7d\n", finfo[i].size);
				for (j = 0; j < 8; j++) {
					s[j] = finfo[i].name[j];
				}
				s[ 9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_exit(struct CONSOLE *cons, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) 0x0fec);
	if (cons->sht != 0) {
		timer_cancel(cons->timer);
	}
	memman_free_4k(memman, (int) fat, 4 * 2880);
	io_cli();
	if (cons->sht != 0) {
		fifo32_put(fifo, cons->sht - shtctl->sheets0 + 768);	/* 768～1023 */
	} else {
		fifo32_put(fifo, task - taskctl->tasks0 + 1024);	/* 1024～2023 */
	}
	io_sti();
	for (;;) {
		task_sleep(task);
	}
}

void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	/* コマンドラインに入力された文字列を、一文字ずつ新しいコンソールに入力 */
	for (i = 6; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_ncst(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct TASK *task = open_constask(0, memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	/* コマンドラインに入力された文字列を、一文字ずつ新しいコンソールに入力 */
	for (i = 5; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_langmode(struct CONSOLE *cons, char *cmdline)
{
	struct TASK *task = task_now();
	unsigned char mode = cmdline[9] - '0';
	if (mode <= 2) {
		task->langmode = mode;
	} else {
		cons_putstr0(cons, "mode number error.\n");
	}
	cons_newline(cons);
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline, char *cmdpath)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	char name[0x100], *p, *q;
	struct TASK *task = task_now();
	int i, segsiz, datsiz, esp, dathrb, appsiz;
	struct SHTCTL *shtctl;
	struct SHEET *sht;

	/* コマンドラインからファイル名を生成 */
	for (i = 0; i < 0xff; i++) {
		if (cmdline[i] == '\0') {
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0; /* とりあえずファイル名の後ろを0にする */
	/* ファイルを探す */
	printk("%s",name);
	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		/* 見つからなかったので後ろに".HRB"をつけてもう一度探してみる */
		name[i    ] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	}
	if (finfo != 0) {
		/* ファイルが見つかった場合 */
		appsiz = finfo->size;
		p = file_loadfile2(finfo->clustno, &appsiz, fat);
		printk("%02X %02X %02X %02X %02X %02X %02X %02X\r\n%02X %02X %02X %02X %02X %02X %02X %02X",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]);
		if (appsiz >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) {
			segsiz = *((int *) (p + 0x0000));
			esp    = *((int *) (p + 0x000c));
			datsiz = *((int *) (p + 0x0010));
			dathrb = *((int *) (p + 0x0014));
			q = (char *) memman_alloc_4k(memman, segsiz);
			task->ds_base = (int) q;
			set_segmdesc(task->ldt + 0, appsiz - 1, (int) p, AR_CODE32_ER + 0x60);
			set_segmdesc(task->ldt + 1, segsiz - 1, (int) q, AR_DATA32_RW + 0x60);
			for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[dathrb + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
			shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if ((sht->flags & 0x11) == 0x11 && sht->task == task) {
					/* アプリが開きっぱなしにした下じきを発見 */
					sheet_free(sht);	/* 閉じる */
				}
			}
			for (i = 0; i < 8; i++) {	/* クローズしてないファイルをクローズ */
				if (task->fhandle[i].buf != 0) {
					memman_free_4k(memman, (int) task->fhandle[i].buf, task->fhandle[i].size);
					task->fhandle[i].buf = 0;
				}
			}
			int i;
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
			timer_cancelall(&task->fifo);
			memman_free_4k(memman, (int) q, segsiz);
			task->langbyte1 = 0;
		} else {
			cons_putstr0(cons, ".hrb file format error.\n");
		}
		memman_free_4k(memman, (int) p, appsiz);
		cons_newline(cons);
		return 1;
	}
	/* ファイルが見つからなかった場合 */
	return 0;
}

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK *task = task_now();
	int ds_base = task->ds_base;
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht;
	struct FIFO32 *sys_fifo = (struct FIFO32 *) *((int *) 0x0fec);
	int *reg = &eax + 1;	/* eaxの次の番地 */
		/* 保存のためのPUSHADを強引に書き換える */
		/* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */
	int i;
	struct FILEINFO *finfo;
	struct FILEHANDLE *fh;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TIME *time1 = time2TIME();

	if (edx == 1) {
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		cons_putstr0(cons, (char *) ebx + ds_base);
	} else if (edx == 3) {
		cons_putstr1(cons, (char *) ebx + ds_base, ecx);
	} else if (edx == 4) {
		return &(task->tss.esp0);
	} else if (edx == 5) {
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= 0x10;
		sheet_setbuf(sht, (char *) ebx + ds_base, esi, edi, eax);
		make_window8((char *) ebx + ds_base, esi, edi, (char *) ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top); /* 今のマウスと同じ高さになるように指定： マウスはこの上になる */
		reg[7] = (int) sht;
	} else if (edx == 6) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + ds_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		memman_init((struct MEMMAN *) (ebx + ds_base));
		ecx &= 0xfffffff0;	/* 16バイト単位に */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 9) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 16バイト単位に切り上げ */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx);
	} else if (edx == 10) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 16バイト単位に切り上げ */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 11) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		hrb_api_linewin(sht, eax, ecx, esi, edi, ebp);
		if ((ebx & 1) == 0) {
			if (eax > esi) {
				i = eax;
				eax = esi;
				esi = i;
			}
			if (ecx > edi) {
				i = ecx;
				ecx = edi;
				edi = i;
			}
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 14) {
		sheet_free((struct SHEET *) ebx);
	} else if (edx == 15) {
		for (;;) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax != 0) {
					task_sleep(task);	/* FIFOが空なので寝て待つ */
				} else {
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons->sht != 0) { /* カーソル用タイマ */
				/* アプリ実行中はカーソルが出ないので、いつも次は表示用の1を注文しておく */
				timer_init(cons->timer, &task->fifo, 1); /* 次は1を */
				timer_settime(cons->timer, 50);
			}
			if (i == 2) {	/* カーソルON */
				cons->cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* カーソルOFF */
				cons->cur_c = -1;
			}
			if (i == 4) {	/* コンソールだけを閉じる */
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);	/* 2024～2279 */
				cons->sht = 0;
				io_sti();
			}
			if (i >= 256) { /* キーボードデータ（タスクA経由）など */
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) {
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->flags2 = 1;	/* 自動キャンセル有効 */
	} else if (edx == 17) {
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) {
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) {
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) {
		for (i = 0; i < 8; i++) {
			if (task->fhandle[i].buf == 0) {
				break;
			}
		}
		fh = &task->fhandle[i];
		reg[7] = 0;
		if (i < 8) {
			finfo = file_search((char *) ebx + ds_base,
					(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
			if (finfo != 0) {
				reg[7] = (int) fh;
				fh->size = finfo->size;
				fh->pos = 0;
				fh->buf = file_loadfile2(finfo->clustno, &fh->size, task->fat);
			}
		}
	} else if (edx == 22) {
		fh = (struct FILEHANDLE *) eax;
		memman_free_4k(memman, (int) fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) {
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			fh->pos = ebx;
		} else if (ecx == 1) {
			fh->pos += ebx;
		} else if (ecx == 2) {
			fh->pos = fh->size + ebx;
		}
		if (fh->pos < 0) {
			fh->pos = 0;
		}
		if (fh->pos > fh->size) {
			fh->pos = fh->size;
		}
	} else if (edx == 24) {
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			reg[7] = fh->size;
		} else if (ecx == 1) {
			reg[7] = fh->pos;
		} else if (ecx == 2) {
			reg[7] = fh->pos - fh->size;
		}
	} else if (edx == 25) {
		fh = (struct FILEHANDLE *) eax;
		for (i = 0; i < ecx; i++) {
			if (fh->pos == fh->size) {
				break;
			}
			*((char *) ebx + ds_base + i) = fh->buf[fh->pos];
			fh->pos++;
		}
		reg[7] = i;
	} else if (edx == 26) {
		i = 0;
		for (;;) {
			*((char *) ebx + ds_base + i) =  task->cmdline[i];
			if (task->cmdline[i] == 0) {
				break;
			}
			if (i >= ecx) {
				break;
			}
			i++;
		}
		reg[7] = i;
	} else if (edx == 27) {
		reg[7] = task->langmode;
	} else if (edx == 28) {
		reg[7] = (int *)time1;
	} else if (edx == 29) {
		reg[7] = time();
	} 
	return 0;
}

int *inthandler0c(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0C :\n Stack Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* 異常終了させる */
}

int *inthandler0d(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0D :\n General Protected Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* 異常終了させる */
}

void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col)
{
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) {
		dx = - dx;
	}
	if (dy < 0) {
		dy = - dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) {
			dx = -1024;
		} else {
			dx =  1024;
		}
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		if (y0 > y1) {
			dy = -1024;
		} else {
			dy =  1024;
		}
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}

	return;
}

int hrb_dpi(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	int *reg = &eax + 1;
	//if(!VEcode(ecx))//����ֻ����ϵͳ���� 
	//{
	//	return -1; 
	//} 
	char s[2];
	struct TASK *task = task_now();
	int ds_base = task->ds_base;
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "t");
	switch(edx)
	{
		case 1:
			io_out8(eax,ecx);
			break;
		case 2:
			reg[7] = io_in8(eax);
			break;
		case 3:
			io_out16(eax,ecx);
			break;
		case 4:
			reg[7] = io_in16(eax);
			break;	
		case 5:
			io_out32(eax,ecx);
			break;
		case 6:
			reg[7] = io_in32(eax);
			break;
		case 7:
			reg[7] = message_send(eax,(struct MESSAGE *)ecx);
			break;
		case 8:
			break;
		case 9:
			s[0]=ebx&0xff;
			s[1]=0;
			putfonts8_asc(binfo->vram, binfo->scrnx, eax, eax, COL8_FFFFFF, s);
			break;
		case 10:
			io_cli(); 
			task_delete(task); 
			task_switchsub();
			task = task_now();
			if(task->devflag) asm("movl %%esp,%0":"=r"(task->tss.esp0)); 
			io_sti();
			farjmp(0, task->sel);
		default:
			return -1;
	}
	return 0;
}
