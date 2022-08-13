/*
	Name: driver.h
	Copyright: 
	Author: Allen
	Date: 13/08/22 11:21
	Description: driver base header
*/

#ifndef DRIVER_DRIVER_H
#define DRIVER_DRIVER_H

#define driver_init(name) \
int HsysMain(int code) \
{ \
	return name(); \
} \
char unused123

#endif	//!DRIVER_DRIVER_H
