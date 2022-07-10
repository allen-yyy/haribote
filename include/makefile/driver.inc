TOOLPATH = ../../z_tools/
INCPATH  = ../../z_tools/haribote/
INCPATH2  = ../application/include/
APILIBPATH   = ../apilib/
CLIBPATH   = ../application/clib/
DPILIBPATH   = ../dpilib/
HARIBOTEPATH = ../haribote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH2) -I../ -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
BIM2SYS  = $(TOOLPATH)bim2sys.exe
BIM2BIN  = $(TOOLPATH)bim2bin.exe
RULEFILE = ../haribote.rul
RULEFILE2= ../haribote2.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
GOLIB    = $(TOOLPATH)golib00.exe 
COPY     = copy
DEL      = del


default :
	$(MAKE) $(APP).sys

$(APP).bim : $(objs) $(APILIBPATH)apilib.lib Makefile ../app_make.txt
	$(OBJ2BIM) @$(RULEFILE) out:$(APP).bim map:$(APP).map stack:$(STACK) \
		$(APP).obj $(APILIBPATH)apilib.lib $(CLIBPATH)haribotelibc.lib

%.gas : %.c ../apilib.h Makefile ../app_make.txt
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile ../app_make.txt
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile ../app_make.txt
	$(NASK) $*.nas $*.obj $*.lst

%.org : %.bim Makefile ../app_make.txt
	$(BIM2HRB) $*.bim $*.org $(MALLOC)
	
%.sys : %.bim Makefile ../app_make.txt
	$(BIM2SYS) $*.bim $*.sys $(MALLOC)

%.hrb : %.org Makefile ../app_make.txt
	$(BIM2BIN) -osacmp in:$*.org out:$*.hrb


run :
	$(MAKE) haribote.img
	$(COPY) haribote.img ..\..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../../z_tools/qemu

full :
	$(MAKE) -C $(APILIBPATH)
	$(MAKE) $(APP).hrb

run_full :
	$(MAKE) -C $(APILIBPATH)
	$(MAKE) -C ../haribote
	$(MAKE) run

clean :
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.org
	-$(DEL) haribote.img

src_only :
	$(MAKE) clean
	-$(DEL) $(APP).hrb

