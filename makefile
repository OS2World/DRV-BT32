# $Id: makefile,v 1.14 2004/02/22 23:03:20 smilcke Exp $

#===================================================================
# Main makefile for BT8X8 32Bit device driver (BT32.SYS)
#===================================================================

#===================================================================
# Setup environment
#===================================================================
.PHONY:	all msg
SHELL	=	$(COMSPEC)

MAKEOPTS	:= -wr

ifdef DEBUG
DEBUG		:= 1
MAKEOPTS	+= DEBUG=1
endif
NOOS2LXAPI	:=

export

all: msg .\makes\paths.mak
	@call $(MAKE) -C lib32 $(MAKEOPTS) all
	@call $(MAKE) -C bttv $(MAKEOPTS) all
	@call $(MAKE) -C dev32 $(MAKEOPTS) all

msg:
	@echo **********************************************************************
ifdef DEBUG
	@echo Building DEBUG Version
else
	@echo Building RELEASE Version
endif
	@echo **********************************************************************

#install:
#	@cd install
#	@$(MAKE) $(MAKEOPTS)
#	@cd ..

.\makes\paths.mak:
	@call tools\config .\makes\paths.mak

clean:
	@echo Cleaning up directories ...
	@call $(MAKE) -C lib32 $(MAKEOPTS) clean
	@call $(MAKE) -C bttv $(MAKEOPTS) clean
	@call $(MAKE) -C dev32 $(MAKEOPTS) clean
	@tools\config .\makes\paths.mak > nul
