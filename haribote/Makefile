OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj \
		int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj \
		mtask.obj window.obj console.obj file.obj tek.obj hd.obj fs.obj driver.obj \
		code.obj bmp.obj alloca.obj acpi.obj init.obj FAT12.obj list.obj pci.obj \
		int32.obj ofifo.obj 

TOOLPATH = ../../z_tools/
INCPATH  = ../../z_tools/haribote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
BIM2BIN  = $(TOOLPATH)bim2bin.exe
RULEFILE = ../haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
GOLIB    = $(TOOLPATH)golib00.exe 
COPY     = copy
DEL      = del
NASM	 = ../nasm.exe

# デフォルト動作

default :
	$(MAKE) ipl09.bin
	$(MAKE) mbr.bin
	$(MAKE) haribote.sys

# ファイル生成規則

ipl09.bin : ipl09.nas Makefile
	$(NASK) ipl09.nas ipl09.bin ipl09.lst

mbr.bin : mbr.nas Makefile
	$(NASK) mbr.nas mbr.bin mbr.lst

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3190k map:bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

haribote.sys : asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb haribote.sys

realcall.obj : #realcall.nas Makefile
	#$(NASM) -o D:\BaiduNetdiskDownload\30ﾌ・ﾔﾖﾆｲﾙﾗ�ﾏｵﾍｳ\ｹ簀ﾌ\tolset\haribote\haribote\realcall.obj -f coff D:\BaiduNetdiskDownload\30ﾌ・ﾔﾖﾆｲﾙﾗ�ﾏｵﾍｳ\ｹ簀ﾌ\tolset\haribote\haribote\realcall.nas

naskfunc.obj : naskfunc.nas Makefile
	$(NASM) -o naskfunc.obj -f coff naskfunc.nas

# 一般規則

%.gas : %.c bootpack.h Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst
	
%.obj : %.asm Makefile
	$(NASM) -o $*.obj -f coff $*.asm

# コマンド

clean :
	-$(DEL) asmhead.bin
	-$(DEL) hankaku.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.hrb

src_only :
	$(MAKE) clean
	-$(DEL) ipl09.bin
	-$(DEL) haribote.sys
