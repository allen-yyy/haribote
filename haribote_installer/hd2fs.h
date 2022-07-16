/*
	Name: hd2fs.h
	Copyright: 
	Author: Allen
	Date: 03/09/21 18:22
	Description: Hd r/w method header.
*/
#ifndef HD2FS_H
#define HD2FS_H
struct hd2fs_message{
	int islba:1;
	int lba;
	int command;
	int sector;
	int isbuf:1;   //Do this command need buf?
	int drive:1;
	int unused:1;
};
struct fs_message{
	int offset;
	int command;
	char *name;
	int size;
	void *buf;
	char letter;
	int handle;
	int mode;
};
#endif // HD2FS_H
