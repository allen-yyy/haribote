#include "bootpack.h"
#include "ide.h" 
#include <stdio.h>

extern struct dev_callon *devcalls[10];

void readhddisk(int driver,int sector,int lba,char *buf)
{
	WaitForBsy(IDE_CTRL0_PORT_STATUS,0); 
	WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
	io_out8(0x1f2, sector);	 


	io_out8(0x1f3, lba);		 
	io_out8(0x1f4, lba >> 8);		 
	io_out8(0x1f5, lba >> 16);		 

    io_out8(0x1f6, 0xa0 | 0x40 | (driver == 1 ? 0x10 : 0) | lba >> 24);
    io_out8(0x1f7, 0x20);
   
    WaitForDrq(IDE_CTRL0_PORT_STATUS,0);
    inws(buf,sector*256,0x1f0);
    return;
}

void writehddisk(int driver,int sector,int lba,char *buf)
{
	WaitForBsy(IDE_CTRL0_PORT_STATUS,0); 
	WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
    io_out8(0x1f2, sector);	 

    io_out8(0x1f3, lba);		 
    io_out8(0x1f4, lba >> 8);		 
    io_out8(0x1f5, lba >> 16);		 

    io_out8(0x1f6, 0xa0 | 0x40 | (driver == 1 ? 0x10 : 0) | lba >> 24);
    io_out8(0x1f7, 0x40);
    WaitForDrq(IDE_CTRL0_PORT_STATUS,0);
    outws(buf,sector*256,0x1f0);
    return;
}

BOOL WaitForRdy(WORD wPort,DWORD dwMillionSecond)
{
	BYTE Status;
	BOOL bResult = FALSE;
	int cnt = 65536;

	while(TRUE)
	{
		Status = io_in8(wPort);
		if(DRIVER_READY(Status))
		{
			bResult = TRUE;
			break;
		}
		cnt --;
		if(0 == cnt)
		{
			break;
		}
	}
	return TRUE;
}

BOOL WaitForBsy(WORD wPort,DWORD dwMillionSecond)
{
	BYTE Status;
	int cnt = 65536;
	BOOL bResult = FALSE;
	
	while(TRUE)
	{
		Status = io_in8(wPort);  //Read status register.
		if(CONTROLLER_READY(Status))
		{
			bResult = TRUE;
			break;
		}
		cnt --;
		if(0 == cnt)
		{
			break;
		}
	}
	return bResult;
}

BOOL WaitForDrq(WORD wPort,DWORD dwMillionSecond)
{
	BYTE Status = 0;
	BOOL bResult = FALSE;
	int cnt = 65536;

	while(TRUE)
	{
		Status = io_in8(wPort);
		if(Status & 8)
		{
			bResult = TRUE;
			break;
		}
		cnt --;
		if(0 == cnt)
		{
			break;
		}
	}
	return bResult;
}

void identify_hd(int driver,char *buf)
{
	WaitForBsy(IDE_CTRL0_PORT_STATUS,0); 
	WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
    io_out8(0x1f6, 0xa0 | 0x40 | (driver == 1 ? 0x10 : 0));
    io_out8(0x1f7, 0xec);
    WaitForDrq(IDE_CTRL0_PORT_STATUS,0);
    inws(buf,512,IDE_CTRL0_PORT_DATA);
    return;
}

void task_hd()
{
	struct MESSAGE message;
	struct hd2fs_message *h2f;
	{
		message_receive(ANY,&message);
		h2f = message.expar;
		switch(message.type)
		{
			case HD_OPEN:
				if(h2f->command!=HD_OPEN||!h2f->isbuf)
				{
					message.params = (char )0;
					message_send(message.src,&message);
					break;	
				}
				identify_hd(0,message.params);
				message_send(message.src,&message);
				break;
			case HD_IDENTIFY:
				if(h2f->command!=HD_IDENTIFY||!h2f->isbuf)
				{
					message.params = (char )0;
					message_send(message.src,&message);
					break;	
				}
				identify_hd(0,message.params);
				message_send(message.src,&message);
				break;
			case HD_READ:
				if(h2f->command!=HD_READ||!h2f->isbuf)
				{
					message.params = (char )0;
					message_send(message.src,&message);
					break;	
				}
				readhddisk(h2f->drive,h2f->sector,h2f->lba,message.params);
				message_send(message.src,&message);
				break;
			case HD_WRITE:
				if(h2f->command!=HD_WRITE||!h2f->isbuf)
				{
					message.params = (char )0;
					message_send(message.src,&message);
					break;	
				}
				writehddisk(h2f->drive,h2f->sector,h2f->lba,message.params);
				message_send(message.src,&message);
				break;
			default:
				message.params = (char )0;
				message_send(message.src,&message);	
		}
	}
	return;
}

BOOL IdeInitialize()
{
	WaitForBsy(IDE_CTRL0_PORT_STATUS,0);
	WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
	io_out8(0x0E,IDE_CTRL0_PORT_CTRL);    //Reset controller,disable interrupt.
	WaitForBsy(IDE_CTRL0_PORT_STATUS,0);
	WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
	return TRUE;
}

void inthandler2e(int *esp)
{
	io_out8(PIC1_OCW2, 0x66);
	io_out8(PIC0_OCW2, 0x62);
	io_in8(IDE_CTRL0_PORT_STATUS); 
	return;
}

BOOL HDEntry(struct Dobject *Dobj)
{
	struct TASK *hdtask;
			
	hdtask = task_alloc();
	hdtask->tss.esp = memman_alloc_4k(Dobj->memman, 64 * 1024) + 64 * 1024;
	hdtask->tss.eip = (int) &task_hd;
	hdtask->tss.es = 1 * 8;
	hdtask->tss.cs = 2 * 8;
	hdtask->tss.ss = 1 * 8;
	hdtask->tss.ds = 1 * 8;
	hdtask->tss.fs = 1 * 8;
	hdtask->tss.gs = 1 * 8;
	task_run(hdtask, 2, 1);
	Dobj->task = hdtask;
	struct dev_callon hdcallon;
	hdcallon.id = IDE_HD_CALLON;
	hdcallon.pid = task2pid(hdtask);
	devcalls[IDE_HD_CALLON] = &hdcallon;
	return TRUE;
} 


/*debug1:hd pid
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	sprintf(s,"taskrun %d",devcalls[IDE_HD_CALLON]->pid);
	putfonts8_asc(binfo->vram, binfo->scrnx, 20, 20, COL8_FFFFFF, s);

*/
