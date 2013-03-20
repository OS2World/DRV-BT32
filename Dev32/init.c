/* $Id: init.c,v 1.30 2004/02/05 00:13:38 smilcke Exp $ */

/*
 * init.c
 * Autor:               Stefan Milcke
 * Erstellt am:         07.09.2001
 * Letzte Aenderung am: 05.02.2004
 *
*/
#ifdef __cplusplus
extern "C" {
#endif

#define INCL_NOPMAPI
#define INCL_DOSINFOSEG
#include <os2.h>

#include <devhlp32.h>
#include <devtype.h>
#include <devrp.h>
#include <ldefos2.h>
#include "devown.h"
#include "Ver_32.h"

#include <bttvos2.h>

#include <lxapilib.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <lxrmcall.h>
#include <..\bttv\bttvp.h>

#include <kee.h>

#include <lxapilib.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

extern int verbose;
extern int verbose_modulelist;
extern int nocoremodule;
extern int nomodule;

extern unsigned long build_level;

static char ERR_ERROR[] = "ERROR: ";
static char ERR_LOCK[] = "Unable to lock segment ";
static char ERR_CS[] = "CODE32.";
static char ERR_DS[] = "DATA32.";
static char ERR_EXIT[] = " Exiting...\r\r\n";
static char ERR_SIGN[]  = "BT32 support driver V%d.%d Build %d bttv(%d.%d.%d) \r\n(C) 2001-2004 by Stefan Milcke\r\n";
static char BTCARD[]    = "Detected card :";
static char TUNER[]     = "Detected tuner:";
static char NONEDETECTED[] = "No BT8X8 compatible device detected\r\n";
static char LOADEDMODULE[] = "Loaded module:";
static char NOMODULE[] = "No module loaded. Use BT32CFG to configure driver.\r\n";
static char ERR_LXAPI32[] = "Unable to attach to LXAPI32.SYS\r\n";
static char VDEC[]      = "(%d.%d.%d)";
static char CRLF[]      = "\r\n";

typedef struct {
 USHORT MsgLength;
 WORD32 MsgPtr;
} MSG_TABLE;

extern unsigned long bttv_ver_major=BT32_DRV_MAJOR_VERSION;
extern unsigned long bttv_ver_minor=BT32_DRV_MINOR_VERSION;

extern WORD32 MSG_TABLE32;

//------------------------------- DevSaveMessage -------------------------------
//Print messages with DosWrite when init is done or has failed (see startup.asm)
void DevSaveMessage(char __far *str, int length)
{
 MSG_TABLE __far *msg = (MSG_TABLE __far *)MAKE_FARPTR32(MSG_TABLE32);
 char __far *str16 = (char __far *)MAKE_FARPTR32(msg->MsgPtr);
 int i;
 for(i=0;i<length;i++)
  str16[msg->MsgLength + i] = str[i];
 str16[msg->MsgLength + length] = 0;
 msg->MsgLength += length;
 return;
}

//-------------------------------- WriteString ---------------------------------
VOID WriteString(const char __far* str, int length)
{
 // Write the string
 DevSaveMessage((char __far *)str, length);
 return;
}

char csLock[12*10]={0};
char dsLock[12*10]={0};
ULONG numCSLock=0;
ULONG numDSLock=0;

//-------------------------------- writeLockErr --------------------------------
void writeLockErr(char* msg,int rc)
{
 char buffer[100];
 WriteString(ERR_ERROR, sizeof(ERR_ERROR)-1);
 sprintf(buffer,"%d ",rc);
 WriteString(buffer,strlen(buffer));
 WriteString(ERR_LOCK, sizeof(ERR_LOCK)-1);
 WriteString(msg,strlen(msg));
 WriteString(ERR_EXIT, sizeof(ERR_EXIT)-1);
}

//-------------------------------- LockSegments --------------------------------
static int LockSegments(void)
{
 int rc=0;
 /* Locks DATA32 into physical memory */
 rc=OS2_lock_mem((VOID*)OffsetBeginDS32,(OffsetFinalDS32 - OffsetBeginDS32)
                 ,VMDHL_LONG | VMDHL_WRITE
                 ,(void*)dsLock,&numDSLock);
 if(rc)
 {
  writeLockErr(&(ERR_DS[0]),rc);
  return rc;
 }
 /* Locks CODE32 into physical memory */
 rc=OS2_lock_mem((VOID*)OffsetBeginCS32,(OffsetFinalCS32 - OffsetBeginCS32)
                 ,VMDHL_LONG
                 ,(void*)csLock,&numCSLock);
 if(rc)
  writeLockErr(&(ERR_CS[0]),rc);
 return rc;
}

//------------------------------- UnLockSegments -------------------------------
static void UnLockSegments(void)
{
 OS2_unlock_mem(csLock,numCSLock);
 OS2_unlock_mem(dsLock,numDSLock);
}

extern void parseArgs(RPInit __far *rp);
extern char *bttv_get_name(int nr);
extern int bttv_num;

//----------------------------- module prototypes ------------------------------
extern int bttv_init_module(void);
extern void bttv_cleanup_module(void);
extern int msp3400_init_module(void);
extern void msp3400_cleanup_module(void);
extern int tda7432_init(void);
extern void tda7432_fini(void);
extern int tda9875_init(void);
extern void tda9875_cleanup_module(void);
extern int tda9887_init_module(void);
extern void tda9887_cleanup_module(void);
extern int audiochip_init_module(void);
extern void audiochip_cleanup_module(void);
extern int tuner_init_module(void);
extern void tuner_cleanup_module(void);

//----------------------------- extern definitions -----------------------------
extern struct os2lx_parm *bttvdrv_parms;
extern int num_bttvdrv_parms;
extern struct os2lx_parm *tuner_parms;
extern int num_tuner_parms;
extern struct os2lx_parm *msp3400_parms;
extern int num_msp3400_parms;
extern struct os2lx_parm *tda7432_parms;
extern int num_tda7432_parms;
extern struct os2lx_parm *tda9887_parms;
extern int num_tda9887_parms;
extern struct os2lx_parm *tvaudio_parms;
extern int num_tvaudio_parms;

DECLARE_LX_MODULE(bttv,BTTV_VERSION_CODE,bttv_init_module,bttv_cleanup_module,bttvdrv_parms);
DECLARE_LX_MODULE(tvaudio,KERNEL_VERSION(2,6,1),audiochip_init_module,audiochip_cleanup_module,tvaudio_parms);
DECLARE_LX_MODULE(tuner,KERNEL_VERSION(2,6,1),tuner_init_module,tuner_cleanup_module,tuner_parms);
DECLARE_LX_MODULE(msp3400,KERNEL_VERSION(2,6,1),msp3400_init_module,msp3400_cleanup_module,msp3400_parms);
DECLARE_LX_MODULE(tda7432,KERNEL_VERSION(2,6,1),tda7432_init,tda7432_fini,tda7432_parms);
DECLARE_LX_MODULE_NP(tda9875,KERNEL_VERSION(2,6,1),tda9875_init,tda9875_cleanup_module);
DECLARE_LX_MODULE(tda9887,KERNEL_VERSION(2,6,1),tda9887_init_module,tda9887_cleanup_module,tda9887_parms);

extern int getBuildDay(void);
extern int getBuildMonth(void);
extern int getBuildYear(void);
extern unsigned long hrmBttvDriver=0;
extern unsigned long hrmBttvAdapter=0;

//------------------------------ DiscardableInit -------------------------------
WORD32 DiscardableInit(RPInit __far* rp)
{
 GetTKSSBase();
 rp->Out.FinalCS = 0;
 rp->Out.FinalDS = 0;
 if(OS2_initlxapi())
 {
  WriteString(ERR_LXAPI32,strlen(ERR_LXAPI32));
  return RPDONE | RPERR;
 }
 if(LockSegments())
 {
//  UnLockSegments();
  return RPDONE | RPERR;
 }

 // Add all supported modules
 OS2_add_module(&bttv_module);
 OS2_add_module(&tvaudio_module);
 OS2_add_module(&tuner_module);
 OS2_add_module(&msp3400_module);
 OS2_add_module(&tda7432_module);
 OS2_add_module(&tda9875_module);
 OS2_add_module(&tda9887_module);

 //Do you init here:
// DebugInt3();
 parseArgs(rp);
 {
  unsigned long rc=0;
  DRIVERSTRUCT DriverStruct;
  DriverStruct.DrvrName=(PSZ)"BT32.SYS";
  DriverStruct.DrvrDescript=(PSZ)"Linux BTTV driver";
  DriverStruct.VendorName=(PSZ)"Stefan Milcke";
  DriverStruct.MajorVer=BT32_DRV_MAJOR_VERSION;
  DriverStruct.MinorVer=BT32_DRV_MINOR_VERSION;
  DriverStruct.Date.Year=getBuildYear();
  DriverStruct.Date.Month=getBuildMonth();
  DriverStruct.Date.Day=getBuildDay();
  DriverStruct.DrvrFlags=0;
  DriverStruct.DrvrType=DRT_SERVICE;
  DriverStruct.DrvrSubType=0;
  DriverStruct.DrvrCallback=NULL;
  rc=RM32CreateDriver(&DriverStruct,&hrmBttvDriver);
 }
 if(!nocoremodule)
 {
  request_module("timer");
  request_module("pci_core");
  request_module("i2c_core");
  request_module("videodev");
 }
 if(!nomodule)
 {
  request_module("bttv");
 }
 if(verbose)
 {
  char buffer[256];
  sprintf(buffer
          ,ERR_SIGN
          ,BT32_DRV_MAJOR_VERSION
          ,BT32_DRV_MINOR_VERSION
          ,BUILD_LEVEL
          ,(BTTV_VERSION_CODE >> 16) & 0xff
          ,(BTTV_VERSION_CODE >> 8) & 0xff
          ,BTTV_VERSION_CODE & 0xff);
  WriteString(buffer,strlen(buffer));
  if(!nomodule)
  {
   if(bttv_num)
   {
    if(verbose_modulelist)
    {
     int n,i;
     n=OS2_get_num_modules();
     for(i=0;i<n;i++)
     {
      char *pName=NULL;
      unsigned long vcode=OS2_get_module_version_code(i);
      WriteString(LOADEDMODULE,strlen(LOADEDMODULE));
      pName=OS2_get_module_name(i);
      WriteString(pName,strlen(pName));
      sprintf(buffer
              ,VDEC
              ,(vcode>>16) & 0xff
              ,(vcode>>8)  & 0xff
              ,(vcode)     & 0xff);
      WriteString(buffer,strlen(buffer));
      WriteString(CRLF,strlen(CRLF));
     }
    }
    WriteString(BTCARD,strlen(BTCARD));
    strcpy(buffer,OS2_bttv_get_name(bttv_num-1));
    WriteString(buffer,strlen(buffer));
    WriteString(CRLF,strlen(CRLF));
    WriteString(TUNER,strlen(TUNER));
    strcpy(buffer,OS2_bttv_get_tuner_name(bttv_num-1));
    WriteString(buffer,strlen(buffer));
    WriteString(CRLF,strlen(CRLF));
   }
   else
   {
    WriteString(NONEDETECTED,strlen(NONEDETECTED));
   }
  }
  else
  {
   WriteString(NOMODULE,strlen(NOMODULE));
  }
 }
 // Complete the installation
 rp->Out.FinalCS = _OffsetFinalCS16;
 rp->Out.FinalDS = _OffsetFinalDS16;

 // Confirm a successful installation
 return RPDONE;
}

#ifdef __cplusplus
}
#endif

