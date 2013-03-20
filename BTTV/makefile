# $Id: makefile,v 1.18 2004/02/23 20:06:40 smilcke Exp $

#===================================================================
# Makefile for BT32 driver
#===================================================================
DEVTYPE    = BTTV
TARGET     = BTTV
HELLOMSG	= Building $(TARGET)

#==============================================================================
# Setup environment
#==============================================================================
.PHONY:	all

#==============================================================================
# Include Main Makefile
#==============================================================================
include $(LXAPI32DEV)\makes\mainmake.mak

#===================================================================
#
#   List of source files
#
#===================================================================
OBJSFIRST:=
OBJS     :=bttv-cards.obj\
           bttv-driver.obj\
           bttv-if.obj\
           msp3400.obj\
           tda7432.obj\
           tda9875.obj\
           tda9887.obj\
           tvaudio.obj\
           tuner.obj\
           bttvos2.obj
OBJLAST  :=


OBJS	  :=$(OBJSFIRST) $(OBJS) $(OBJSLAST)

LNKFILE  := $(DRV_OBJ)\$(TARGET).lrf

#===================================================================
#
#   Specific dependencies
#
#===================================================================
all: ShowHelloMsg MakeBinDir $(DRV_LIB)\$(TARGET).lib makefile $(COMMONDEPENDENCIES)

$(DRV_LIB)\$(TARGET).lib: $(OBJSFIRST) $(OBJS) $(OBJSLAST) $(LNKFILE)
	@echo --------------------- $@
	@if exist $(DRV_LIB)\$(TARGET).lib del $(DRV_LIB)\$(TARGET).lib
	$(LIBTOOL) $(DRV_LIB)\$(TARGET).lib @$(LNKFILE) ,;
	@echo "'"

$(LNKFILE): makefile
	@if exist $(LNKFILE) del $(LNKFILE)
	@for %f in ($(OBJS)) do @echo + ""$(DRV_OBJ)\%f'" ^&>> $(LNKFILE)

clean: ShowHelloMsg
	@echo Cleanup...
	@if exist $(DRV_OBJ)\*.obj del $(DRV_OBJ)\*.obj
	@if exist $(DRV_OBJ)\*.lrf del $(DRV_OBJ)\*.lrf
	@if exist $(DRV_OBJ)\*.lst del $(DRV_OBJ)\*.lst
	@if exist $(DRV_SRC)\*.err del $(DRV_SRC)\*.err
	@if exist $(DRV_LIB)\$(TARGET).lib del $(DRV_LIB)\$(TARGET).lib
