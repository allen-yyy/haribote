/* asmhead.nas */
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* ブートセクタはどこまでディスクを読んだのか */
	char leds; /* ブート時のキーボードのLEDの状態 */
	char vmode; /* ビデオモード  何ビットカラーか */
	char reserve;
	short scrnx, scrny; /* 画面解像度 */
	char *vram;
};
#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG		0x00100000

/* types */
#define BYTE                char
#define CHAR                char
#define TCHAR               short
#define UCHAR               unsigned char
#define UBYTE               unsigned char
#define WORD                unsigned short
#define UWORD               unsigned short
#define DWORD               unsigned long
#define LPSTR               char*
#define LPCTSTR             const char*
#define INT                 int
#define UINT                unsigned int
#define FLOAT               float
#define DOUBLE              double

#define __U8                unsigned char
#define __U16               unsigned short
#define __U32               unsigned int

#define BOOL                DWORD
#define FALSE               0x00000000
#define TRUE                0x00000001
#define NULL                0x00000000
#define MAX_DWORD_VALUE     0xFFFFFFFF
#define MAX_WORD_VALUE      0xFFFF
#define MAX_BYTE_VALUE      0xFF
#define MAX_QWORD_VALUE     0xFFFFFFFFFFFFFFFF

#define VOID                void
#define LPVOID              void*

#define LOWORD(dw)          WORD(dw)
#define HIWORD(dw)          WORD(dw >> 16)

#define LOBYTE(wr)          BYTE(wr)
#define HIBYTE(wr)          BYTE(wr >> 16)

#include "list.h"

/* naskfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
void asm_inthandler0c(void);
void asm_inthandler0d(void);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler2c(void);
void asm_inthandler2e(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void asm_hrb_api(void);
void asm_hrb_dpi(void);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void start_sys(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);
void clts(void);
void fnsave(int *addr);
void frstor(int *addr);
void asm_inthandler07(void);
void inws(char *b,long len,short port);
void outws(char *b,long len,short port);
 	VOID WriteByteToPort(UCHAR, WORD);

	//
	//Read string of data from port.
	//
	VOID ReadByteStringFromPort(LPVOID, DWORD, WORD);

	//
	//Write string data to port.
	//
	VOID WriteByteStringToPort(LPVOID, DWORD, WORD);

	VOID ReadWordFromPort(WORD* pWord, WORD  wPort);

	VOID WriteWordToPort(WORD, WORD);

	VOID ReadWordStringFromPort(LPVOID, DWORD, WORD);

	VOID WriteWordStringToPort(LPVOID, DWORD, WORD);
	
#define BI3 int asm_inthandler##n(int *esp);
#define BI2  BI3(0x20) BI3(0x21) BI3(0x22) BI3(0x23) BI3(0x24) BI3(0x25) \
BI3(0x26) BI3(0x27) BI3(0x28) BI3(0x29) BI3(0x2a) \
BI3(0x2b) BI3(0x2c) BI3(0x2d) BI3(0x2e) BI3(0x2f)

int asm_inthandler0x20(int *esp);
int asm_inthandler0x21(int *esp);
int asm_inthandler0x22(int *esp);
int asm_inthandler0x23(int *esp);
int asm_inthandler0x24(int *esp);
int asm_inthandler0x25(int *esp);
int asm_inthandler0x26(int *esp);int asm_inthandler0x27(int *esp);
int asm_inthandler0x28(int *esp);int asm_inthandler0x29(int *esp);
int asm_inthandler0x2a(int *esp);int asm_inthandler0x2b(int *esp);
int asm_inthandler0x2c(int *esp);int asm_inthandler0x2d(int *esp);
int asm_inthandler0x2e(int *esp);int asm_inthandler0x2f(int *esp);

/* realcall.nas */
typedef struct __attribute__ ((packed)) {
	unsigned short di, si, bp, sp, bx, dx, cx, ax;
	unsigned short gs, fs, es, ds, eflags;
} regs16_t;
void int32(unsigned char intnum, regs16_t *regs);



/* fifo.c */
struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

/* graphic.c */
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/* dsctbl.c */
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_LDT			0x0082
#define AR_TSS32		0x0089
#define AR_INTGATE32	0x008e

/* int.c */
void init_pic(void);
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define MAX_IRQUSED 5
typedef int (*irqfunc) (int *);

/* keyboard.c */
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(struct FIFO32 *fifo, int data0);
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

/* mouse.c */
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
void inthandler2c(int *esp);
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

/* memory.c */
#define MEMMAN_FREES		4090	/* これで約32KB */
#define MEMMAN_ADDR			0x003c0000
struct FREEINFO {	/* あき情報 */
	unsigned int addr, size;
};
struct MEMMAN {		/* メモリ管理 */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);

void init_screen8(char *vram, int x, int y, int *fat,struct MEMMAN *memman);

/* sheet.c */
#define MAX_SHEETS		256
struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
	struct TASK *task;
};
struct SHTCTL {
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

/* timer.c */
#define MAX_TIMER		500
struct TIMER {
	struct TIMER *next;
	unsigned int timeout;
	char flags, flags2;
	struct FIFO32 *fifo;
	int data;
};
struct TIMERCTL {
	unsigned int count, next;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};
extern struct TIMERCTL timerctl;
void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void inthandler20(int *esp);
int timer_cancel(struct TIMER *timer);
void timer_cancelall(struct FIFO32 *fifo);

/* mtask.c */
#define MAX_TASKS		1000	/* 最大タスク数 */
#define TASK_GDT0		3		/* TSSをGDTの何番から割り当てるのか */
#define MAX_TASKS_LV	100
#define MAX_TASKLEVELS	10
#define MAX_PID			9999
#define task2pid(task)	task->pid
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};
struct TASK {
	int sel, flags; /* selはGDTの番号のこと */
	int level, priority;
	struct FIFO32 fifo;
	struct TSS32 tss;
	int fpu[108 / 4];
	struct SEGMENT_DESCRIPTOR ldt[2];
	struct CONSOLE *cons;
	int ds_base, cons_stack;
	struct FILEHANDLE *fhandle;
	int *fat;
	char *cmdline;
	unsigned char langmode, langbyte1;
	int pid;
	int r_flags,s_flag;
	struct MESSAGE *message_r;
	struct list_elem list_tag;
	int devflag;
	//struct blocks_t *waits;
};
struct TASKLEVEL {
	int running; /* 動作しているタスクの数 */
	int now; /* 現在動作しているタスクがどれだか分かるようにするための変数 */
	struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
	int now_lv; /* 現在動作中のレベル */
	struct TASK *task_fpu;
	char lv_change; /* 次回タスクスイッチのときに、レベルも変えたほうがいいかどうか */
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};
struct pid_t{
	struct ppiiffdd{
		int pid;
		struct TASK *task;
	} pido [MAX_PID];
	int next;
};
struct pid1{
	int pid;
	struct TASK *task;
	struct list_elem list_tag;
};
struct blocks_t{
	struct{
		int pid;
		struct TASK *task;
	}blocko[MAX_TASKS];
	int next;
};
struct MESSAGE{
	int type;
	int *Param;
	char *params;
	int src;
	void *expar; 
}; 
extern struct TASKCTL *taskctl;
extern struct TIMER *task_timer;
struct TASK *task_now(void);
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int level, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);
//void task_sleep(struct TASK *task);
void task_block(struct TASK *task);
void task_unblock(struct TASK *task);
struct TASK *pid2task(int pid);
int *inthandler07(int *esp);

#define ANY 10000

/* window.c */
void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);
void change_wtitle8(struct SHEET *sht, char act);

/* console.c */
struct CONSOLE {
	struct SHEET *sht;
	int cur_x, cur_y, cur_c;
	struct TIMER *timer;
};
struct FILEHANDLE {
	char *buf;
	int size;
	int pos;
};
struct TIME {
	int year;
	int moon;
	int day;
	char hour;
	char min;
	char sec;
};
void console_task(struct SHEET *sheet, int memtotal);
void cons_putchar(struct CONSOLE *cons, int chr, char move);
void cons_newline(struct CONSOLE *cons);
void cons_putstr0(struct CONSOLE *cons, char *s);
void cons_putstr1(struct CONSOLE *cons, char *s, int l);
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, int memtotal);
void cmd_mem(struct CONSOLE *cons, int memtotal);
void cmd_cls(struct CONSOLE *cons);
void cmd_dir(struct CONSOLE *cons);
void cmd_exit(struct CONSOLE *cons, int *fat);
void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal);
void cmd_ncst(struct CONSOLE *cons, char *cmdline, int memtotal);
void cmd_langmode(struct CONSOLE *cons, char *cmdline);
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);
int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
int hrb_dpi(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
int *inthandler0d(int *esp);
int *inthandler0c(int *esp);
void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);
void printtime(struct CONSOLE *cons);
void readrtc(unsigned char *t);

/* file.c */
struct FILEINFO {
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};
void file_readfat(int *fat, unsigned char *img);
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
char *file_loadfile2(int clustno, int *psize, int *fat);

/* tek.c */
int tek_getsize(unsigned char *p);
int tek_decomp(unsigned char *p, char *q, int size);

/* bootpack.c */
struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal);
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal);
#include "init.h"


struct Dobject;
typedef BOOL (*DEntry)(struct Dobject *);


/* hd.c */
#define HD_OPEN 		0x1
#define HD_IDENTIFY 	0x2
#define HD_READ 		0x3
#define HD_WRITE		0x4
#include "hd2fs.h"
void inthandler2e(int *esp);
BOOL Identify(int nHdNum,BYTE* pBuffer);
BOOL CmdSucc(WORD wPort);
BOOL WaitForDrq(WORD wPort,DWORD dwMillionSecond);
BOOL WaitForBsy(WORD wPort,DWORD dwMillionSecond);
BOOL WaitForRdy(WORD wPort,DWORD dwMillionSecond);
BOOL HDEntry(struct Dobject *Dobj);

/* driver.c */
#define DR_NUM 2
#define DR_ENUM 1

#define IDE_HD_CALLON	0
#define FS_CALLON		1
struct Dobject{
	char *name;
	struct MEMMAN *memman;
	struct TASK *task;
	void (*unload)(struct Dobject *this); 
};
enum typeof_dev{
	IDE_HD_DEV = 0,FS_DEV,PCI_DEV,SCSI_DEV
};
struct dDevEntry {
	char *name;
	DEntry entry;
	struct Dobject *Dobj;
};
BOOL LDevs(struct MEMMAN *memman);
BOOL load_external_device(int *fat,struct MEMMAN *memman);
struct Dobject *GetMyObj(char *name);
//typedef BOOL (*DEntry) (Dobject);
struct dev_callon {
	struct list_elem tag;
	int id; 
	int pid;
};

/* fs.c */
#define FS_HDSIZE 0x1
#define FS_READ 0x2
BOOL FSEntry(struct Dobject *Dobj); 

/* code.c */
struct _codecore{
	int next;
	int code[0xff];
};
int code_alloc();
void code_init();
BOOL VEcode(int vcode); 
