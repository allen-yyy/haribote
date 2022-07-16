/*
	Name: pci.c
	Copyright: 
	Author: Allen
	Date: 06/02/21 21:35
	Description: pci driver 
*/

#include "bootpack.h" 
#include "pci.h"
struct list pci_dev_list; 
struct list pci_bus_list;
struct pci_bus *bus_list[0x100];
int busnum = 0;
int devnum = 0;

/* Hardware */

int _pci_read_byte(int bus,int slot,int func,int offset){
	int address = ((bus << 16) | (slot << 11) |(func << 8) | (offset & 0xFC) | 0x80000000);
	io_out32(0xCF8, address);
	return ((io_in32(0xCFC) >> ((offset & 3) * 8)) & 0xFF);
}
int _pci_read_word(int bus,int slot,int func,int offset){
	int address = ((bus << 16) | (slot << 11) |(func << 8) | (offset & 0xFC) | 0x80000000);
	io_out32(0xCF8, address);
	return ((io_in32(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}
int _pci_read_dword(int bus,int slot,int func,int offset){
	int address = ((bus << 16) | (slot << 11) |(func << 8) | (offset & 0xFC) | 0x80000000);
	io_out32(0xCF8, address);
	return (io_in32(0xCFC) & 0xFFFFFFFF);
}

int _pci_write_byte(int bus,int slot,int func,int offset,int value){
	int address = ((bus << 16) | (slot << 11) |(func << 8) | (offset & 0xFC) | 0x80000000);
	io_out32(0xCF8, address);
	io_out8((char)value,0xCFC + (offset & 3));
	return 0;
}
int _pci_write_word(int bus,int slot,int func,int offset,int value){
	int address = ((bus << 16) | (slot << 11) |(func << 8) | (offset & 0xFC) | 0x80000000);
	io_out32(0xCF8, address);
	io_out16((short)value,0xCFC + (offset & 2));
	return 0;
}
int _pci_write_dword(int bus,int slot,int func,int offset,int value){
	int address = ((bus << 16) | (slot << 11) |(func << 8) | (offset & 0xFC) | 0x80000000);
	io_out32(0xCF8, address);
	io_out32(value,0xCFC);
	return 0;
}

int pci_read_byte(struct pci_dev *dev,int offset){
	
	return _pci_read_byte(dev->bus->number,dev->bus_slot,dev->slot_func,offset);
}
int pci_read_word(struct pci_dev *dev,int offset){
	return _pci_read_word(dev->bus->number,dev->bus_slot,dev->slot_func,offset);
}
int pci_read_dword(struct pci_dev *dev,int offset){
	return _pci_read_dword(dev->bus->number,dev->bus_slot,dev->slot_func,offset);
}

int pci_write_byte(struct pci_dev *dev,int offset,int value){
	_pci_write_byte(dev->bus->number,dev->bus_slot,dev->slot_func,offset,value);
	return 0;
}
int pci_write_word(struct pci_dev *dev,int offset,int value){
	_pci_write_word(dev->bus->number,dev->bus_slot,dev->slot_func,offset,value);
	return 0;
}
int pci_write_dword(struct pci_dev *dev,int offset,int value){
	_pci_write_dword(dev->bus->number,dev->bus_slot,dev->slot_func,offset,value);
	return 0;
}

/* Software */

struct pci_dev *pci_add_dev(struct pci_dev *tmp)
{
	struct pci_dev *dev = (struct pci_dev *) kalloc(sizeof(struct pci_dev));
	int i;
	dev->bus = tmp->bus;
	dev->bus_slot = tmp->bus_slot;
	dev->slot_func = tmp->slot_func;
	dev->header_type = tmp->header_type;
	dev->vendor = (unsigned short)_pci_read_word(dev->bus->number,dev->bus_slot,dev->slot_func,PCI_VENDOR_ID);
	dev->device = (unsigned short)_pci_read_word(dev->bus->number,dev->bus_slot,dev->slot_func,PCI_DEVICE_ID);
	dev->class_revision = _pci_read_dword(dev->bus->number,dev->bus_slot,dev->slot_func,PCI_CLASS_REVISION);
	list_append(&pci_dev_list,&dev->tag);
	devnum++;
	//list_append(&dev->bus->devs,&dev->tag);
	return dev;
}

void pci_check_func(int bus,int slot,int func)
{
	struct pci_dev dev;
	dev.bus = bus_list[bus];
	dev.bus_slot = slot;
	dev.slot_func = func;
	dev.header_type = _pci_read_byte(bus,slot,func,PCI_HEADER_TYPE);
	struct pci_dev *dev2 = pci_add_dev(&dev);
	printk("add dev %X %4X %d %d %d 0x%X %d\n",(unsigned short)dev2->vendor,(short)dev2->device,dev2->bus->number,slot,func,dev2->class_revision,devnum);
	if(_pci_read_word(bus,slot,func,PCI_CLASS_DEVICE) == 0x604) //PCI bridge
	{
		pci_scan_bus(_pci_read_byte(bus,slot,func,PCI_SECONDARY_BUS));
	}
	return;
}

void pci_check_dev(int bus,int slot)
{
	int i;
	if(_pci_read_word(bus,slot,0,0)==0xFFFF||_pci_read_word(bus,slot,0,0)==0x0000) return;
	if(_pci_read_byte(bus,slot,0,PCI_HEADER_TYPE)&0x80 != 0)
	{
		for(i=0;i<8;i++)
		{
			if(_pci_read_word(bus,slot,i,0)!=0xFFFF&&_pci_read_word(bus,slot,i,0)!=0x0000)
			{
				pci_check_func(bus,slot,i);
			} 
		}
	}else pci_check_func(bus,slot,0);
	return;
}

void pci_scan_bus(int bus)
{
	int i;
	struct pci_bus pbus;
	pbus.number=bus;
	pbus.primary_bus=bus;
	pbus.secondary_bus=bus;
	bus_list[bus]=&pbus;
	for(i=0;i<32;i++)
	{
		pci_check_dev(bus,i);
	}
	busnum++;
	return;
}

BOOL pci_probe()
{
	if(_pci_read_dword(0,0,0,0)==0xFFFFFFFF) return 0;
	return 1;
}

int pci_init()
{
	if(!pci_probe()) return 0;
	list_init(&pci_bus_list);
	list_init(&pci_dev_list);
	pci_scan_bus(0);
	return 1;
}

BOOL pci_test_f(struct list_elem *tags, int arg)
{
	struct pci_dev *dev = elem2entry(struct pci_dev,tag,tags);
	struct CONSOLE *cons=(struct CONSOLE *)arg;
	char s[1000];
	sprintf(s,"%4x %4x %d %d %d\n",dev->vendor,dev->device,dev->bus->number,dev->bus_slot,dev->slot_func);
	cons_putstr0(cons, s);
	return FALSE;	
}

void pci_test(struct CONSOLE *cons)
{
	list_traversal(&pci_dev_list,pci_test_f,(int) cons);
	//if(list_empty(&pci_dev_list)) cons_putstr0(cons, "empty!!!");
	//else
	//{
		char s[100]; 
		sprintf(s,"%d\n\n",busnum);
		cons_putstr0(cons, s);
	//} 
	return;
}

BOOL PCIEntry(struct Dobject *Dobj)
{
	BOOL a=pci_init();
	//printk("\n%X",_pci_read_dword(0,0,0,0));
	return TRUE;
}
