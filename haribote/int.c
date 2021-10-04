/* ���荞�݊֌W */

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
/* PIC�̏����� */
{
	io_out8(PIC0_IMR,  0xff  ); /* �S�Ă̊��荞�݂��󂯕t���Ȃ� */
	io_out8(PIC1_IMR,  0xff  ); /* �S�Ă̊��荞�݂��󂯕t���Ȃ� */

	io_out8(PIC0_ICW1, 0x11  ); /* �G�b�W�g���K���[�h */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7�́AINT20-27�Ŏ󂯂� */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1��IRQ2�ɂĐڑ� */
	io_out8(PIC0_ICW4, 0x01  ); /* �m���o�b�t�@���[�h */

	io_out8(PIC1_ICW1, 0x11  ); /* �G�b�W�g���K���[�h */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15�́AINT28-2f�Ŏ󂯂� */
	io_out8(PIC1_ICW3, 2     ); /* PIC1��IRQ2�ɂĐڑ� */
	io_out8(PIC1_ICW4, 0x01  ); /* �m���o�b�t�@���[�h */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1�ȊO�͑S�ċ֎~ */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 �S�Ă̊��荞�݂��󂯕t���Ȃ� */

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
