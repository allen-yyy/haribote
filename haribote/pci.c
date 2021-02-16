/*
	Name: pci.c
	Copyright: 
	Author: Allen
	Date: 06/02/21 21:35
	Description: 
*/

#include "bootpack.h" 

static DWORD PciScanBus(DEVICE *lpBridge, DWORD dwBusNum)
{
	DWORD dwLoop = 0;
	DEVICE *lpPhyDev = NULL;
	DWORD dwSubNum = dwBusNum;

	/* Basic checking. */
	//BUG_ON(NULL == lpDevMgr);
	/* 
	 * This routine can be called only in process of system 
	 * initialization,so no locks are obtained when initialize
	 * the physial device list or other global data structures.
	 */
	//BUG_ON(!IN_SYSINITIALIZATION());
	if(255 <= dwBusNum)
	{
		/* Maximal bus number should not exceed 255. */
		return MAX_DWORD_VALUE;
	}

	//for(dwLoop = 0;dwLoop < MAX_BUS_NUM;dwLoop ++)
	//{
	//	if (BUS_TYPE_NULL == lpDevMgr->SystemBus[dwLoop].dwBusType)
	//.	{
	//		break;
	//	}
	//}
	//if(MAX_BUS_NUM == dwLoop)
	//{
	//	/* Can not find a free bus,the number of system buses exceed MAX_BUS_NUM. */
	//	return MAX_DWORD_VALUE;
	//}

	/* Now we have found a free system bus element,initialize it. */
	lpDevMgr->SystemBus[dwLoop].dwBusType = BUS_TYPE_PCI;
	lpDevMgr->SystemBus[dwLoop].dwBusNum = dwBusNum;
	lpDevMgr->SystemBus[dwLoop].lpHomeBridge = lpBridge;
	if(lpBridge)
	{
		/* If the current bus is not root bus. */
		lpDevMgr->SystemBus[dwLoop].lpParentBus = lpBridge->lpHomeBus;
		lpBridge->lpChildBus = &lpDevMgr->SystemBus[dwLoop];
	}
	//Set PCI bus operations.
	lpDevMgr->SystemBus[dwLoop].ReadConfig  = PciReadConfig;
	lpDevMgr->SystemBus[dwLoop].WriteConfig = PciWriteConfig;

	/* Scan all devices on this bus. */
	PciScanDevices(&lpDevMgr->SystemBus[dwLoop]);
	lpPhyDev = lpDevMgr->SystemBus[dwLoop].lpDevListHdr;
	/* Scan all child buses of the current bus. */
	while(lpPhyDev)
	{
		if(PCI_DEVICE_TYPE_BRIDGE == 
			((__PCI_DEVICE_INFO*)lpPhyDev->lpPrivateInfo)->dwDeviceType)
		{
			/* PCI bridge. */
			dwSubNum = PciScanBus(lpDevMgr,
				lpPhyDev,
				((__PCI_DEVICE_INFO*)lpPhyDev->lpPrivateInfo)->ucSecondary);
		}
		lpPhyDev = lpPhyDev->lpNext;
	}
	return dwSubNum;
}
