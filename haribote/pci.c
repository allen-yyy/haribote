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
int busnum;

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
	
	return _pci_read_byte(dev->bus_num,dev->bus_slot,dev->slot_func,offset);
}
int pci_read_word(struct pci_dev *dev,int offset){
	return _pci_read_word(dev->bus_num,dev->bus_slot,dev->slot_func,offset);
}
int pci_read_dword(struct pci_dev *dev,int offset){
	return _pci_read_dword(dev->bus_num,dev->bus_slot,dev->slot_func,offset);
}

int pci_write_byte(struct pci_dev *dev,int offset,int value){
	_pci_write_byte(dev->bus_num,dev->bus_slot,dev->slot_func,offset,value);
	return 0;
}
int pci_write_word(struct pci_dev *dev,int offset,int value){
	_pci_write_word(dev->bus_num,dev->bus_slot,dev->slot_func,offset,value);
	return 0;
}
int pci_write_dword(struct pci_dev *dev,int offset,int value){
	_pci_write_dword(dev->bus_num,dev->bus_slot,dev->slot_func,offset,value);
	return 0;
}

/* Software */

struct pci_dev *pci_add_dev(struct pci_dev *tmp)
{
	struct pci_dev *dev = (struct pci_dev *) kalloc(sizeof(struct pci_dev));
	int i;
	dev->bus_num = tmp->bus_num;
	dev->bus_slot = tmp->bus_slot;
	dev->slot_func = tmp->slot_func;
	dev->header_type = tmp->header_type;
	dev->vendor = _pci_read_word(dev->bus->number,dev->bus_slot,dev->slot_func,PCI_VENDOR_ID);
	dev->int_pin = _pci_read_byte(dev->bus->number,dev->bus_slot,dev->slot_func,PCI_DEVICE_ID);
	dev->class_revision = _pci_read_dword(dev->bus->number,dev->bus_slot,dev->slot_func,PCI_CLASS_REVISION);
	list_append(&pci_dev_list,dev->tag);
	list_append(dev->bus->devs,dev->tag);
	return dev;
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
	return 1;
}

BOOL PCIEntry(struct Dobject *Dobj)
{
	return pci_init();
}
