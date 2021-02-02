#include "bootpack.h"
#include "hd.h"


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

BOOL IdeInitialize()
{
	WaitForBsy(IDE_CTRL0_PORT_STATUS,0);
	WaitForRdy(IDE_CTRL0_PORT_STATUS,0);
	__outb(0x0E,IDE_CTRL0_PORT_CTRL);    //Reset controller,disable interrupt.
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

BOOL HDEntry()
{
	DWORD dwLba;
	UCHAR Buff[512];
	//IdeInitialize();
	Identify(0,(BYTE*)&Buff[0]);
	dwLba = ((DWORD)Buff[123] << 24) + ((DWORD)Buff[122] << 16) 
			+ ((DWORD)Buff[121] << 8) + (DWORD)Buff[120];
	return TRUE;
} 
