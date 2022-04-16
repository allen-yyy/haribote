/*
	Name: pci.h
	Copyright: 
	Author: Allen
	Date: 19/01/2022 11:59
	Description: pci
*/

struct pci_dev
{
	int bus_num;
	int bus_slot;
	int slot_func;
	char is_multiple;
	char header_type;
	char int_line;
	char int_pin;
	int class_revision;
	struct list_elem tag;
};

struct pci_bus
{
	int number;
	int secondary_bus;
	int primary_bus;
	int dev_num;
	struct pci_dev *devs[256];
	struct list_elem tag;
};
