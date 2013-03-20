# $Id: makefile,v 1.6 2004/02/16 21:24:59 smilcke Exp $

#===================================================================
# Makefile for CARDINFO installation DLL
#===================================================================
TARGET1    = CARDINFO
DEVTYPE    = DEV32
HELPFILE   = VIDHELP

!if [@ECHO ************************]
!endif
!if [@ECHO Building $(TARGET1)]
!endif
!if [@ECHO ************************]
!endif

!include ..\makes\paths.mak

!if [@if not exist bin md bin]
!endif


RC=$(DDK)\Tools\rc
RFLAGS=-i $(DDK)\base\h

IPF=ipfc
L=ENU
P=437
C=1

#===================================================================
# Object file list
#===================================================================
OBJS1     = rcstub.obj

!include $(DRV_MAKES)\envchk.mak
!include $(DRV_MAKES)\common.mak
!include $(LX_MAINMAKE)

LRFFILE1=$(DRV_OBJ)\$(TARGET1).lrf
LRFFILES=$(LRFFILE1)
TARGETFILES=$(DRV_OBJ)\$(TARGET1).dll $(DRV_OBJ)\$(HELPFILE).hlp

all: $(LRFFILES) $(TARGETFILES)

$(LRFFILE1): makefile
   @echo $(DRV_OBJ)\$(TARGET1).DLL > $(DRV_OBJ)\$(TARGET1).lrf
   @echo $(DRV

   @%write $^@ Name $(DRV_OBJ)\$(TARGET1).dll
   @%write $^@ System os2v2 dll
   @%write $^@ Option Description 'BTTV Installation DLL'
   @%write $^@ File $(DRV_OBJ)\$(OBJS1)

$(DRV_OBJ)\$(TARGET1).dll: $(OBJS1) $(TARGET1).rc $(LRFFILE1) makefile
   $(LINK) $(LFLAGS) @$(LRFFILE1)
   $(RC) $(RFLAGS) -r $(TARGET1).rc $(DRV_OBJ)\$(TARGET1).res
   $(RC) $(RFLAGS) $(DRV_OBJ)\$(TARGET1).res $(DRV_OBJ)\$(TARGET1).dll

#help: $(DRV_OBJ)\$(HELPFILE).hlp

$(DRV_OBJ)\$(HELPFILE).hlp: $(HELPFILE).itl makefile
   $(IPF) /l=$(L) /codepage=$(P) /country=$(C) $(HELPFILE).itl $(DRV_OBJ)\$(HELPFILE).hlp

clean: .SYMBOLIC
   @if exist $(DRV_OBJ)\*.obj del $(DRV_OBJ)\*.obj
   @if exist $(DRV_OBJ)\*.lst del $(DRV_OBJ)\*.lst
   @if exist $(DRV_OBJ)\*.lrf del $(DRV_OBJ)\*.lrf
   @if exist $(DRV_OBJ)\*.hlp del $(DRV_OBJ)\*.hlp
   @if exist $(DRV_OBJ)\*.res del $(DRV_OBJ)\*.res
   @if exist $(DRV_OBJ)\*.dll del $(DRV_OBJ)\*.dll
   @if exist $(DRV_SRC)\*.err del $(DRV_SRC)\*.err
