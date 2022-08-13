/* ウィンドウ関係 */

#include "bootpack.h"

void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act)
{
	boxfill8(buf, xsize, COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
	boxfill8(buf, xsize, COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
	boxfill8(buf, xsize, COL8_C6C6C6, 0,         0,         0,         ysize - 1);
	boxfill8(buf, xsize, COL8_FFFFFF, 1,         1,         1,         ysize - 2);
	boxfill8(buf, xsize, COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL8_C6C6C6, 2,         2,         xsize - 3, ysize - 3);
	boxfill8(buf, xsize, COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);
	make_wtitle8(buf, xsize, title, act);
	return;
}

void make_wtitle8(unsigned char *buf, int xsize, char *title, char act)
{
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c, tc, tbc;
	if (act != 0) {
		tc = COL8_FFFFFF;
		tbc = COL8_000084;
	} else {
		tc = COL8_C6C6C6;
		tbc = COL8_848484;
	}
	boxfill8(buf, xsize, tbc, 3, 3, xsize - 4, 20);
	putfonts8_asc(buf, xsize, 24, 4, tc, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l)
{
	struct TASK *task = task_now();
	boxfill8(sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15);
	if (task->langmode != 0 && task->langbyte1 != 0) {
		putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
		sheet_refresh(sht, x - 8, y, x + l * 8, y + 16);
	} else {
		putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
		sheet_refresh(sht, x, y, x + l * 8, y + 16);
	}
	return;
}

void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c)
{
	int x1 = x0 + sx, y1 = y0 + sy;
	boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
	boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
	boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
	boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, c,           x0 - 1, y0 - 1, x1 + 0, y1 + 0);
	return;
}

void make_button8(struct SHEET *sht, int x0, int y0, int c, char *name, struct FIFO32 *fifo)
{
	char *buf = sht->buf;
	int len = 0,i,j,x,y;
	for(len=0;;len++) if(name[len]=='\0') break;
	static char btna[16][16] = {
		"OOOOOOOOOOOOOOOO",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"OQQQQQQQQQQQQQQQ",
		"O$$$$$$$$$$$$$$$",
		"@@@@@@@@@@@@@@@@"
	};
	char btnb[16][16] = {
		"OOOOOOOOOOOOOOOO",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"QQQQQQQQQQQQQQQQ",
		"$$$$$$$$$$$$$$$$",
		"@@@@@@@@@@@@@@@@"
	};
	static char btnc[16][16] = {
		"OOOOOOOOOOOOOOO@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"QQQQQQQQQQQQQQ$@",
		"$$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			c = btna[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(x0+x)+(y0+y)*sht->bxsize] = c;
		}
	}
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			c = btnb[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			btnb[y][x] = c;
		}
	}
	for(i=0;i<len/2-2;i++)
	{
		for (y = 0; y < 16; y++) {
			for (x = 0; x < 16; x++) {
				buf[(x0+16+x+i*16)+(y0+y)*sht->bxsize] = btnb[y][x];
			}
		}
	}
	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			c = btnc[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(x0+(len-2)*8+x)+(y0+y)*sht->bxsize] = c;
		}
	}
	putfonts8_asc(buf, sht->bxsize, x0, y0, COL8_FFFFFF, name);
	sheet_refresh(sht, 0, 0, sht->bxsize, sht->bysize);
	mouse_click_register(sht, x0, y0, x0+len*16, y0+16, fifo);
	return;
}

void make_dialog(unsigned char *buf, char *title, char *msg, int button)
{
	int xsize,ysize;
	xsize=ysize=128;
	boxfill8(buf, xsize, COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
	boxfill8(buf, xsize, COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
	boxfill8(buf, xsize, COL8_C6C6C6, 0,         0,         0,         ysize - 1);
	boxfill8(buf, xsize, COL8_FFFFFF, 1,         1,         1,         ysize - 2);
	boxfill8(buf, xsize, COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL8_C6C6C6, 2,         2,         xsize - 3, ysize - 3);
	boxfill8(buf, xsize, COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);
	make_wtitle8(buf, xsize, title, 0);
	return;
}

void change_wtitle8(struct SHEET *sht, char act)
{
	int x, y, xsize = sht->bxsize;
	char c, tc_new, tbc_new, tc_old, tbc_old, *buf = sht->buf;
	if (act != 0) {
		tc_new  = COL8_FFFFFF;
		tbc_new = COL8_000084;
		tc_old  = COL8_C6C6C6;
		tbc_old = COL8_848484;
	} else {
		tc_new  = COL8_C6C6C6;
		tbc_new = COL8_848484;
		tc_old  = COL8_FFFFFF;
		tbc_old = COL8_000084;
	}
	for (y = 3; y <= 20; y++) {
		for (x = 3; x <= xsize - 4; x++) {
			c = buf[y * xsize + x];
			if (c == tc_old && x <= xsize - 22) {
				c = tc_new;
			} else if (c == tbc_old) {
				c = tbc_new;
			}
			buf[y * xsize + x] = c;
		}
	}
	sheet_refresh(sht, 3, 3, xsize, 21);
	return;
}
