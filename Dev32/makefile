# $Id: makefile,v 1.32 2004/02/23 20:06:41 smilcke Exp $

#===================================================================
# Makefile for BT32 driver
#===================================================================
TARGET     = BT32
DEVTYPE    = DEV32
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
OBJSFIRST:=header.obj
OBJS     :=strmstub.obj\
           strategy.obj\
           idc.obj\
           ioctl.obj\
           param.obj\
           blddat.obj\
           stream.obj\
           strmioctl.obj\
           strmidc.obj\
           strmcmd.obj\
           strmthrd.obj\
           init.obj
OBJSLAST :=end.obj

OBJS	  :=$(OBJSFIRST) $(OBJS)

LRFFILE  :=$(DRV_OBJ)\$(TARGET).lrf
LINUXLIB :=$(DRV_LIB)\linux.lib
MAPNAME  :=$(DRV_BIN)\$(TARGET).MAP
DEFFILE  :=$(DRV_OBJ)\$(TARGET).def

#===================================================================
#
#   Specific dependencies
#
#===================================================================
all: ShowHelloMsg MakeBinDir $(DRV_BIN)\$(TARGET).sys $(DRV_BIN)\$(TARGET).sym makefile $(COMMONDEPENDENCIES)

$(LRFFILE): makefile $(COMMONDEPENDENCIES)
	@if not exist $(DRV_BIN) @md $(DRV_BIN)
	@echo PHYSICAL DEVICE > $(DEFFILE)
	@echo DESCRIPTION '@#$(_VENDOR):$(_VERSION).$(BUILD_LEVEL)#@ BT8X8 Support driver' >> $(DEFFILE)

	@echo /OUT:$(DRV_BIN)\$(TARGET).sys > $(LRFFILE)
	@echo /MAP:$(DRV_BIN)\$(TARGET).map >> $(LRFFILE)
	@echo $(DEFFILE) >> $(LRFFILE)
	@for %f in ($(OBJS)) do @echo ""$(DRV_OBJ)\%f'" >> $(LRFFILE)
	@for %f in ($(LXAPIHLPOBJS)) do @echo ""%f >> $(LRFFILE)
	@echo $(LINUXLIB) >> $(LRFFILE)
	@echo $(DRV_LIB)\bttv.lib >> $(LRFFILE)
	@echo $(LX_LIB)\impdos.lib >> $(LRFFILE)
	@echo $(WATCOM)\lib386\os2\clib3r.lib >> $(LRFFILE)
	@for %f in ($(OBJSLAST)) do @echo ""$(DRV_OBJ)\%f'" >> $(LRFFILE)

$(DRV_BIN)\$(TARGET).sys: $(LRFFILE) $(OBJS) $(OBJSLAST) $(LINUXLIB) $(DRV_LIB)\bttv.lib
	@echo -------------------- $(TARGET).sys
	@$(BLDMGR) /FILE:$(LRFFILE) /TEXT:@#$(_VENDOR):$(_VERSION). /ACTION:SET:$(BUILD_LEVEL)
	$(LNKTOOL) @$(LRFFILE)

$(DRV_BIN)\$(TARGET).sym: $(DRV_BIN)\$(TARGET).SYS
	@echo --------------------- $(TARGET).sym
	mapsym $(MAPNAME)
	@copy $(TARGET).SYM $(DRV_BIN)\$(TARGET).sym > nul
	@if exist $(TARGET).SYM del $(TARGET).SYM > nul

clean: ShowHelloMsg
	@echo Cleanup...
	@if exist $(DRV_OBJ)\*.obj del $(DRV_OBJ)\*.obj
	@if exist $(DRV_OBJ)\*.lrf del $(DRV_OBJ)\*.lrf
	@if exist $(DRV_OBJ)\*.lst del $(DRV_OBJ)\*.lst
	@if exist $(DRV_SRC)\*.err del $(DRV_SRC)\*.err
	@if exist $(DRV_BIN)\*.def del $(DRV_BIN)\*.def
	@if exist $(DRV_BIN)\$(TARGET).* del $(DRV_BIN)\$(TARGET).*
