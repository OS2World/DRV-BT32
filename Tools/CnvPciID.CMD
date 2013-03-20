/* $Id: CnvPciID.CMD,v 1.7 2002/05/01 01:19:41 smilcke Exp $ */

/* Generates files from pci.ids
   Copyright (c) 2001-2001 by Stefan Milcke
                              KÅferstra·e 45
                              28779 Bremen

   Datum der Erstellung: 06.10.2001
   Letzte Aenderung am:  06.10.2001
*/
Call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
Call SysLoadFuncs;

Parse Arg dFile sFile .

If Length(dFile)<1 Then
Do
 Say "Usage: CnvPciID.CMD <TARGET_FILE> [<PCIID_FILE>]"
 Say "       TARGET_FILE = Target file (pci-nc.c for BT drivers)"
 Say "       PCIID_FILE  = List of pci id's (normally pci.ids)"
 Exit
End

If Length(Stream(dFile,'C','QUERY EXISTS'))>0 Then Call SysFileDelete dFile
rc=Stream(dFile,'C','OPEN WRITE')
If Length(sFile)<1 Then
Do
 rc=WritePartFromCmd(dFile,'common_head')
 rc=WritePartFromCmd(dFile,'foot_nonames')
 rc=WritePartFromCmd(dFile,'common_foot')
 rc=Stream(dFile,'C','CLOSE')
 Exit
End
rc=Stream(sFile,'C','OPEN READ')

/* Write structure definitions */
rc=WritePartFromCmd(dFile,'common_head')
rc=WritePartFromCmd(dFile,'head_fullnames')

vendor.0=0
device.0=0
subsys.0=0
class.0=0
dn=0

Line=NextValidLine(sFile,'')

Do While(Lines(sFile))
 If Lines(sFile)>0 Then
 Do
  If Left(Line,1)='C' Then
  Do
   Line=NextValidLine(sFile,LineIn(sFile))
   Do While Left(Line,1)=D2C(9)
    Line=NextValidLine(sFile,LineIn(sFile))
   End
  End
  Else
  Do
   v=vendor.0+1
   vendor.0=v
   vendor.v.id='0x'Left(Line,4)
   vendor.v.name=Translate(Right(Line,Length(Line)-6),',','"')
   vendor.v.firstd=dn
   vendor.v.lastd=dn
   Line=NextValidLine(sFile,LineIn(sFile))
   Do While(Lines(sFile) & Left(Line,1)=D2C(9) )
    Do While Left(Line,2)=D2C(9)D2C(9)
     s=subsys.0+1
     subsys.0=s
     subsys.s.vendorid=vendor.v.id
     subsys.s.deviceid=device.d.id
     subsys.s.vid=Right(Left(Line,6),4)
     subsys.s.did=Right(Left(Line,10),4)
     subsys.s.name=Translate(Right(Line,Length(Line)-13),',','"')
     Line=NextValidLine(sFile,LineIn(sFile))
    End
    d=device.0+1
    dn=dn+1
    vendor.v.lastd=dn
    device.0=d
    device.d.vendorid=vendor.v.id
    device.d.id='0x'Right(Left(Line,5),4)
    device.d.name=Translate(Right(Line,Length(Line)-7),',','"')
    Line=NextValidLine(sFile,LineIn(sFile))
   End
  End
 End
End

rc=LineOut(dFile,'/* List of all known devices */')
rc=LineOut(dFile,'static struct pci_device_info devices[]= {')
Do i=1 To device.0
 rc=LineOut(dFile,' {'device.i.id',0,"'device.i.name'"},')
End
rc=LineOut(dFile,'};')
rc=LineOut(dFile,'')
rc=LineOut(dFile,'/* List of all known vendors */')
rc=LineOut(dFile,'static struct pci_vendor_info vendors[]= {')
Do i=1 To vendor.0
 If vendor.i.lastd=vendor.i.firstd Then
 Do
  rc=LineOut(dFile,' {'vendor.i.id',0,"'vendor.i.name'",0,0},')
 End
 Else
 Do
  rc=LineOut(dFile,' {'vendor.i.id',0,"'vendor.i.name'",'vendor.i.lastd-vendor.i.firstd',&(devices['vendor.i.firstd'])},')
 End
End
rc=LineOut(dFile,'};')

rc=WritePartFromCmd(dFile,'foot_fullnames')
rc=WritePartFromCmd(dFile,'common_foot')

Exit

NextValidLine: Procedure
 sFile=Arg(1)
 Line=Arg(2)
 Do While( Lines(sFile) & (Length(Line)=0 | Left(Line,1)='#') )
  Line=LineIn(sFile)
 End
Return Line

WritePartFromCmd: Procedure
 dFile=Arg(1)
 delim=Arg(2)
 rc=0
 Do i=1 To SourceLine()
  If SourceLine(i)='<'delim'>' Then
  Do
   rc=1
   i=i+1
   Do While (SourceLine(i)<>'</'delim'>') & i<SourceLine()
    rc=LineOut(dFile,SourceLine(i))
    i=i+1
   End
   Leave
  End
 End
Return rc

/* The following lines are copied to the generated source */
<common_head>
/*
 * pci-nc.c
 * Autor:    Automatic generation from CnvPciID.CMD
 *
 *
*/

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/init.h>

struct pci_device_info
{
 unsigned short device;
 unsigned short seen;
 const char *name;
};

struct pci_vendor_info
{
 unsigned short vendor;
 unsigned short nr;
 const char *name;
 unsigned short num_devices;
 struct pci_device_info *devices;
};

</common_head>

<head_fullnames>
#ifdef FULLPCINAMES_INTERNALLY
</head_fullnames>

<foot_fullnames>

#define VENDORS (sizeof(vendors)/sizeof(struct pci_vendor_info))
#define DEVICES (sizeof(devices)/sizeof(struct pci_device_info))
static int num_pci_device_infos=DEVICES;
static int num_pci_vendor_infos=VENDORS;

#else

static struct pci_device_info *devices=0;
static struct pci_vendor_info *vendors=0;
static int num_pci_device_infos=0;
static int num_pci_vendor_infos=0;

#endif // FULLPCINAMES

</foot_fullnames>

<foot_nonames>
static struct pci_device_info *devices=0;
static struct pci_vendor_info *vendors=0;
static int num_pci_device_infos=0;
static int num_pci_vendor_infos=0;

</foot_nonames>

<common_foot>
void pci_name_device_init(char __far *pci_list_file)
{
}

void pci_name_device_free(void)
{
}

void pci_name_device(struct pci_dev *dev)
{
 int i=0;
 struct pci_device_info *pdev=(struct pci_device_info *)0;
 struct pci_vendor_info *pven=vendors;
 for(i=0;i<num_pci_vendor_infos;i++,pven++)
  if(pven->vendor==dev->vendor)
   goto match_vendor;
 sprintf(dev->name,"PCI device %04x:%04x",dev->vendor,dev->device);
 return;
match_vendor:
 {
  pdev=pven->devices;
  for(i=0;i<pven->num_devices;i++)
  {
   if(pdev->device==dev->device)
    goto match_device;
   pdev++;
  }
  sprintf(dev->name,"PCI device %04x:%04x (%s)",dev->vendor,dev->device,pven->name);
  return;
 }
match_device:
 sprintf(dev->name,"%s %s",pven->name,pdev->name);
 pdev->seen=pdev->seen+1;
}

</common_foot>
