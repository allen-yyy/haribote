/* 割り込み関係 */

#include "bootpack.h"
#include <stdio.h>

struct irq_s{
	int num;
	int ms:1;	//PIC1 or PCI0
	int numdriver;
	irqfunc functions[MAX_IRQUSED];
	int type;
};

struct irq_s irq[16];

void init_pic(void)
/* PICの初期化 */
{
	io_out8(PIC0_IMR,  0xff  ); /* 全ての割り込みを受け付けない */
	io_out8(PIC1_IMR,  0xff  ); /* 全ての割り込みを受け付けない */

	io_out8(PIC0_ICW1, 0x11  ); /* エッジトリガモード */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7は、INT20-27で受ける */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1はIRQ2にて接続 */
	io_out8(PIC0_ICW4, 0x01  ); /* ノンバッファモード */

	io_out8(PIC1_ICW1, 0x11  ); /* エッジトリガモード */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15は、INT28-2fで受ける */
	io_out8(PIC1_ICW3, 2     ); /* PIC1はIRQ2にて接続 */
	io_out8(PIC1_ICW4, 0x01  ); /* ノンバッファモード */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1以外は全て禁止 */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 全ての割り込みを受け付けない */

	int i;
	for(i=0;i<8;i++)
	{
		irq[i].num = i;
		irq[i].ms = 0;
		irq[i].numdriver = 0;
		irq[i].type = -1;
	}
	for(i=8;i<16;i++)
	{
		irq[i].num = i;
		irq[i].ms = 1;
		irq[i].numdriver = 0;
		irq[i].type = -1;
	}
	return;
}

void inthandlers(int *esp,char irqn)
{
	struct irq_s info = irq[irqn];
	int i;
	if(info.ms)
	{
		io_out8(PIC1_OCW2, 0x20);
		io_out8(PIC0_OCW2, 0x20);
	}else
		io_out8(PIC0_OCW2, 0x20);
	if(info.numdriver==0) return;
	for(i=0;i<info.numdriver;i++)
	{
		if(!(info.functions[i])(esp))
			break;
	}
	return;
}

int request_irq(int irqn,irqfunc irqhander,int type1)
{
	struct irq_s info = irq[irqn];
	if(info.numdriver>=MAX_IRQUSED) return -1;
	if(info.type==1) return -1;
	if(type1==1&&info.numdriver>0) return -1;
	info.numdriver++;
	info.functions[info.numdriver-1] = irqhander;
	info.type=type1; 
	return 0;
}
