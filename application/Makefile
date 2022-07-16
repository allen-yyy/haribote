#	New Makefile for Haribote.
#	Add Driver,Application
#	Haribote version 0.2B

MAKE     = $(TOOLPATH)make.exe -r

include apps.make

default:
	$(foreach n,$(APPS),$(MAKE) -C $(n))