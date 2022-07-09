/*
	Name: time.h
	Copyright: 
	Author: Allen
	Date: 11/09/21 19:08
	Description: api time
*/
#ifndef TIME_H
#define TIME_H

#ifndef TIME_DEF
#define TIME_DEF
struct TIME {
	int year;
	int moon;
	int day;
	char hour;
	char min;
	char sec;
};
#endif //TIME_DEF

int time();	//unix time
struct TIME *gettime();

#endif //TIME_H
