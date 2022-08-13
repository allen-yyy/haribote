#	New Makefile for Haribote.
#	Add Driver,Application
#	Haribote version 0.2B

TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/haribote/

MAKE     = $(TOOLPATH)make.exe -r
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
BIM2BIN  = $(TOOLPATH)bim2bin.exe
COPY     = copy
DEL      = del

include application/apps.make
include driver/drivers.make
include fonts/fonts.make

OBJS-y   = 

default :
	$(MAKE) img

img :	haribote $(OBJS) Makefile application driver include fonts
	$(MAKE) -C haribote
	$(MAKE) -C driver
	$(MAKE) -C application
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:haribote/ipl09.bin len:512 from:0 to:0 \
		copy from:haribote/haribote.sys to:@: \
		$(foreach n,$(APPS),copy from:application/$(n)/$(n).hrb to:@: ) \
		$(foreach n,$(DRIVERS),copy from:driver/$(n)/$(n).sys to:@: ) \
		$(foreach n,$(FONTS),copy from:fonts/$(n)/$(n).fnt to:@: ) \
		imgout:haribote.img

install :	haribote haribote_installer $(OBJS) Makefile application driver include fonts
	$(MAKE) -C haribote
	$(MAKE) -C driver
	$(MAKE) -C application
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:haribote/mbr.bin len:512 from:0 to:0 \
		copy from:haribote/haribote.sys to:@: \
		$(foreach n,$(APPS),copy from:application/$(n)/$(n).hrb to:@: ) \
		$(foreach n,$(DRIVERS),copy from:driver/$(n)/$(n).sys to:@: ) \
		$(foreach n,$(FONTS),copy from:fonts/$(n)/$(n).fnt to:@: ) \
		imgout:install.org
	$(MAKE) -C haribote_installer
	$(BIM2BIN) -osacmp in:install.org out:install.img
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:haribote_installer/ipl09.bin len:512 from:0 to:0 \
		copy from:haribote_installer/haribote.sys to:@: \
		copy from:install.img to:@: \
		imgout:installer.img
