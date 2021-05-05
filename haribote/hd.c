#include "bootpack.h"
#include "hd.h"
#include "ide.h" 
#include <stdio.h>

BOOL ReadHDSector(LPVOID lpBuffer,
				  UCHAR byStartSector,
				  UCHAR byCylinderLo,
				  UCHAR byCylinderHi,
				  UCHAR byCtrlDrvHdr)     //bit 0 - 3 : Header.
				                          //bit 4     : Driver.
										  //bit 5 - 7 : Controller.
{
	BOOL          bResult      = FALSE;
	DWORD         dwCounter    = 0x00000000;
	UCHAR         byFlags      = 0x00;
	UCHAR         byDrvHdr     = 0x00;

	if(NULL == lpBuffer)
		return bResult;
	if(byCtrlDrvHdr >= 32)               //Currently,the system can not support
		                                 //more than one controller.
        return bResult;

	//---------- ** debug ** -------------
	WriteByteToPort(0x00,IDE_CTRL0_PORT_CTRL);  //Build the controller method.

	while(dwCounter < 0xFFFFFFFF)       //Wait for controller and driver ready.
	{
		byFlags=io_in8(IDE_CTRL0_PORT_STATUS);
		if(!CONTROLLER_READY(byFlags) || !DRIVER_READY(byFlags))
		{
			dwCounter ++;
			continue;
		}
		break;
	}
	if(0xFFFFFFFF == dwCounter)        //Time out.
	{
		//PrintLine("The controller or driver is not ready in waiting time.");
		return bResult;
	}

	switch(16 & byCtrlDrvHdr)         //Determine which driver is to be accessed.
	{
	case 0:
		byDrvHdr = FORM_DRIVER_HEADER(IDE_DRV0_LBA,byCtrlDrvHdr);
		break;
	case 16:
		byDrvHdr = FORM_DRIVER_HEADER(IDE_DRV1_LBA,byCtrlDrvHdr);
		break;
	default:
		return bResult;
		break;
	}

	WriteByteToPort(0x4b,IDE_CTRL0_PORT_PRECOMP);
	WriteByteToPort(0x01,IDE_CTRL0_PORT_SECTORNUM);
	WriteByteToPort(byStartSector,IDE_CTRL0_PORT_STARTSECTOR);
	WriteByteToPort(byCylinderLo,IDE_CTRL0_PORT_CYLINDLO);
	WriteByteToPort(byCylinderHi,IDE_CTRL0_PORT_CYLINDHI);
	WriteByteToPort(byDrvHdr,IDE_CTRL0_PORT_HEADER);
	WriteByteToPort(IDE_CMD_READ,IDE_CTRL0_PORT_CMD);

	while(TRUE)
	{
		byFlags = io_in8(IDE_CTRL0_PORT_STATUS);
		if(CONTROLLER_READY(byFlags))
			break;
	}

    ReadWordStringFromPort(lpBuffer,512,IDE_CTRL0_PORT_DATA);

	bResult = TRUE;
	return bResult;
}

BOOL WriteHDSector(LPVOID lpBuffer,
				  UCHAR byStartSector,
				  UCHAR byCylinderLo,
				  UCHAR byCylinderHi,
				  UCHAR byCtrlDrvHdr)     //bit 0 - 3 : Header.
				                          //bit 4     : Driver.
										  //bit 5 - 7 : Controller.
{
	BOOL          bResult      = FALSE;
	DWORD         dwCounter    = 0x00000000;
	UCHAR         byFlags      = 0x00;
	UCHAR         byDrvHdr     = 0x00;

	if(NULL == lpBuffer)
		return bResult;
	if(byCtrlDrvHdr >= 32)               //Currently,the system can not support
		                                 //more than one controller.
        return bResult;

	//---------- ** debug ** -------------
	WriteByteToPort(0x00,IDE_CTRL0_PORT_CTRL);  //Build the controller method.

	while(dwCounter < 0xFFFFFFFF)       //Wait for controller and driver ready.
	{
		byFlags = io_in8(IDE_CTRL0_PORT_STATUS);
		if(!CONTROLLER_READY(byFlags) || !DRIVER_READY(byFlags))
		{
			dwCounter ++;
			continue;
		}
		break;
	}
	if(0xFFFFFFFF == dwCounter)        //Time out.
	{
		//PrintLine("The controller or driver is not ready in waiting time.");
		return bResult;
	}

	switch(16 & byCtrlDrvHdr)         //Determine which driver is to be accessed.
	{
	case 0:
		byDrvHdr = FORM_DRIVER_HEADER(IDE_DRV0_LBA,byCtrlDrvHdr);
		break;
	case 16:
		byDrvHdr = FORM_DRIVER_HEADER(IDE_DRV1_LBA,byCtrlDrvHdr);
		break;
	default:
		return bResult;
		break;
	}

	WriteByteToPort(0x4b,IDE_CTRL0_PORT_PRECOMP);
	WriteByteToPort(0x01,IDE_CTRL0_PORT_SECTORNUM);
	WriteByteToPort(byStartSector,IDE_CTRL0_PORT_STARTSECTOR);
	WriteByteToPort(byCylinderLo,IDE_CTRL0_PORT_CYLINDLO);
	WriteByteToPort(byCylinderHi,IDE_CTRL0_PORT_CYLINDHI);
	WriteByteToPort(byDrvHdr,IDE_CTRL0_PORT_HEADER);
	WriteByteToPort(IDE_CMD_WRITE,IDE_CTRL0_PORT_CMD);

	while(TRUE)
	{
		byFlags = io_in8(IDE_CTRL0_PORT_STATUS);
		if(CONTROLLER_READY(byFlags))
			break;
	}

    WriteWordStringToPort(lpBuffer,512,IDE_CTRL0_PORT_DATA);

	bResult = TRUE;
	return bResult;
}

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
	io_out8(IDE_CTRL0_PORT_HEADER,DeviceReg);
	//WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
	io_out8(IDE_CTRL0_PORT_CMD,IDE_CMD_IDENTIFY);
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
	io_cli();
	for(;;)
	{
		char s[31];
		i++;
		struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
		//---DEBUG--- 
		//io_cli(); 
		//boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
		//sprintf(s,"taskrun %d",1);
		//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
		 
		message_receive(ANY,message);
		//boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
		//sprintf(s,"taskrun %d",2);
		//putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
		switch(message->type)
		{
			case HD_OPEN:
				Identify(0,message->params);
				boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
				sprintf(s,"pid is %d",message->src);
				putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
				message_send(message->src,message);
				break;
			case HD_IDENTIFY:
				Identify(0,message->params);
				message_send(message->src,message);
				break;
			default:
				message->params = (char *)0;
				message_send(message->src,message);	
		}
		//io_hlt(); 
	}
	io_sti();
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
	char s[31]; 
	DWORD dwLba;
	UCHAR Buff[512];
	//IdeInitialize();
	Identify(0,(BYTE*)&Buff[0]);
	//UINT sectors = *(UINT*)&Buff[60*2];
			
	hdtask = task_alloc();
	hdtask->tss.esp = memman_alloc_4k(Dobj->memman, 64 * 1024) + 64 * 1024;
	hdtask->tss.eip = (int) &task_hd;
	hdtask->tss.es = 1 * 8;
	hdtask->tss.cs = 2 * 8;
	hdtask->tss.ss = 1 * 8;
	hdtask->tss.ds = 1 * 8;
	hdtask->tss.fs = 1 * 8;
	hdtask->tss.gs = 1 * 8;
	task_run(hdtask, 3, 2);
	
	*((int *) 0x0f01) = hdtask;
	Dobj->task = hdtask;
	//---DEBUG--- 
	//io_cli();
	//io_sti();
	return TRUE;
} 
