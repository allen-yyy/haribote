#include "bootpack.h"
#include "hd.h"
#include "ide.h" 
#include <stdio.h>

//Several low level routines to test specified flags in hard disk driver registers.
static BOOL WaitForRdy(WORD wPort,DWORD dwMillionSecond)
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

static BOOL WaitForBsy(WORD wPort,DWORD dwMillionSecond)
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

static BOOL WaitForDrq(WORD wPort,DWORD dwMillionSecond)
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

static BOOL CmdSucc(WORD wPort)
{
	BYTE status = io_in8(wPort);
	if(COMMAND_SUCC(status))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL Identify(int nHdNum,BYTE* pBuffer)
{
	BYTE DeviceReg = 0x0;  //Device select register.
	BYTE Status    = 0;  //Status register.

	if(nHdNum >= 2)  //Not supported yet.
	{
		return FALSE;
	}
	if(nHdNum == 1)
	{
		DeviceReg += 16;  //Select the second hard disk.
	}
	//Issue the IDENTIFY DEVICE command.
	WaitForBsy(IDE_CTRL0_PORT_STATUS,0);  //Wait for controller to free.
	WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
	io_out8(DeviceReg,IDE_CTRL0_PORT_HEADER);
	//WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
	io_out8(IDE_CMD_IDENTIFY,IDE_CTRL0_PORT_CMD);
	//Wait for command completion.
	//WaitForBsy(IDE_CTRL0_PORT_STATUS,0);
	//WaitForRdy(IDE_CTRL0_PORT_STATUS,0);  //-- DEBUG --
	WaitForDrq(IDE_CTRL0_PORT_STATUS,0);
	inws(pBuffer,512,IDE_CTRL0_PORT_DATA);
	if(CmdSucc(IDE_CTRL0_PORT_STATUS))
	{
		return TRUE;
	}
	return FALSE;
}

void task_hd()
{
	struct MESSAGE *message;
	int i=0;
	for(;;)
	{
		char s[31];
		i++;
		struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
		sprintf(s,"taskrun %d",i);
		putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
		message_receive(ANY,message);
		switch(message->type)
		{
			case HD_OPEN:
				Identify(0,message->params);
				message_send(message->src,message);
			default:
				message->params = (char *)0;
				message_send(message->src,message);	
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
	return;
}

BOOL HDEntry(struct Dobject *Dobj)
{
	//struct MEMMAN *memman = *((int *) 0x0ef0);
	struct TASK *hdtask;
	DWORD dwLba;
	UCHAR Buff[512];
	//IdeInitialize();
	Identify(0,(BYTE*)&Buff[0]);
	dwLba = ((DWORD)Buff[123] << 24) + ((DWORD)Buff[122] << 16) 
			+ ((DWORD)Buff[121] << 8) + (DWORD)Buff[120];
			
	hdtask = task_alloc();
	hdtask->tss.esp = memman_alloc_4k(Dobj->memman, 64 * 1024) + 64 * 1024;
	hdtask->tss.eip = (int) &task_hd;
	hdtask->tss.es = 1 * 8;
	hdtask->tss.cs = 2 * 8;
	hdtask->tss.ss = 1 * 8;
	hdtask->tss.ds = 1 * 8;
	hdtask->tss.fs = 1 * 8;
	hdtask->tss.gs = 1 * 8;
	task_run(hdtask, 4, 1);
	
	*((int *) 0x0f01) = hdtask;
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
		//sprintf(s,"taskrun %d",i);
		putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "looooo");

	return TRUE;
} 
