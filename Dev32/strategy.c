/* $Id: strategy.c,v 1.19 2003/08/03 21:16:44 smilcke Exp $ */

/*
 * strategy.c
 * Autor:               Stefan Milcke
 * Erstellt am:         07.09.2001
 * Letzte Aenderung am: 29.07.2003
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
#include "devown.h"

#include <lxapilib.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kmod.h>

extern WORD32 StratIOCtl(RP __far *_rp);
WORD32 StratClose(RP __far *_rp);

// SvL: Needed in StratOpen
WORD32 numOS2Opens = 0;
// SvL: End

static char devFileName[]="bt32.sys";

extern int StrmRegisterVSDInstance(int flags,ULONG vsdpid,ULONG devno);

//--------------------------------- StratOpen ----------------------------------
WORD32 StratOpen(RP __far* _rp)
{
 RPOpenClose __far* rp=(RPOpenClose __far*)_rp;
 OS2_open_device_called(rp->FileID,devFileName);
 numOS2Opens++;
 return RPDONE;
}

//--------------------------------- StratClose ---------------------------------
ULONG StratClose(RP __far* _rp)
{
 ULONG pid=OS2_get_current_pid();
 RPOpenClose __far* rp = (RPOpenClose __far*)_rp;
 // only called if device successfully opened
 numOS2Opens--;
 OS2_close_device_called(rp->FileID);
 StrmRegisterVSDInstance(0,pid,1);
 StrmRegisterVSDInstance(0,pid,2);
 StrmRegisterVSDInstance(0,pid,3);
 StrmRegisterVSDInstance(0,pid,4);
 return(RPDONE);
}

//--------------------------------- StratWrite ---------------------------------
#pragma off (unreferenced)
static WORD32 StratWrite(RP __far* _rp)
#pragma on (unreferenced)
{
  return RPDONE | RPERR;
}

extern WORD32 DiscardableInit(RPInit __far*);

//--------------------------------- StratInit ----------------------------------
WORD32 StratInit(RP __far* _rp)
{
  RPInit __far* rp = (RPInit __far*)_rp;
  return DiscardableInit(rp);
}


//----------------------------- StratInitComplete ------------------------------
#pragma off (unreferenced)
WORD32 StratInitComplete(RP __far* _rp)
#pragma on (unreferenced)
{
  return(RPDONE);
}

//--------------------------------- StratError ---------------------------------
static WORD32 StratError(RP __far*)
{
  return RPERR_COMMAND | RPDONE;
}

//------------------------------- StratDeinstall -------------------------------
#pragma off (unreferenced)
static WORD32 StratDeinstall(RP __far* rp)
#pragma on (unreferenced)
{
 release_module("tda9875");
 release_module("tda7432");
 release_module("msp3400");
 release_module("tuner");
 release_module("tvaudio");
 release_module("bttv");
 return RPDONE;
}

//------------------------------- StratShutdown --------------------------------
static WORD32 StratShutdown(RP __far* _rp)
{
 RPShutdown __far *rp=(RPShutdown __far *)_rp;
 if(rp->Function==0)
 {
  release_module("tda9875");
  release_module("tda7432");
  release_module("msp3400");
  release_module("tuner");
  release_module("tvaudio");
  release_module("bttv");
 }
 return RPDONE;
}

// Strategy dispatch table
//
// This table is used by the strategy routine to dispatch strategy requests

typedef WORD32 (*RPHandler)(RP __far* rp);
RPHandler StratDispatch[] =
{
  StratInit,                  // 00 (BC): Initialization
  StratError,                 // 01 (B ): Media check
  StratError,                 // 02 (B ): Build BIOS parameter block
  StratError,                 // 03 (  ): Unused
  StratError,                 // 04 (BC): Read
  StratError,                 // 05 ( C): Nondestructive read with no wait
  StratError,                 // 06 ( C): Input status
  StratError,                 // 07 ( C): Input flush
  StratWrite,                 // 08 (BC): Write
  StratError,                 // 09 (BC): Write verify
  StratError,                 // 0A ( C): Output status
  StratError,                 // 0B ( C): Output flush
  StratError,                 // 0C (  ): Unused
  StratOpen,                  // 0D (BC): Open
  StratClose,                 // 0E (BC): Close
  StratError,                 // 0F (B ): Removable media check
  StratIOCtl,                 // 10 (BC): IO Control
  StratError,                 // 11 (B ): Reset media
  StratError,                 // 12 (B ): Get logical unit
  StratError,                 // 13 (B ): Set logical unit
  StratDeinstall,             // 14 ( C): Deinstall character device driver
  StratError,                 // 15 (  ): Unused
  StratError,                 // 16 (B ): Count partitionable fixed disks
  StratError,                 // 17 (B ): Get logical unit mapping of fixed disk
  StratError,                 // 18 (  ): Unused
  StratError,                 // 19 (  ): Unused
  StratError,                 // 1A (  ): Unused
  StratError,                 // 1B (  ): Unused
  StratShutdown,              // 1C (BC): Notify start or end of system shutdown
  StratError,                 // 1D (B ): Get driver capabilities
  StratError,                 // 1E (  ): Unused
  StratInitComplete           // 1F (BC): Notify end of initialization
};

//---------------------------------- Strategy ----------------------------------
WORD32 Strategy(RP __far* rp);
#pragma aux (STRATEGY) Strategy "STRATEGY_ENTRY";
WORD32 Strategy(RP __far* rp)
{
 WORD32 rc;
 if (rp->Command < sizeof(StratDispatch)/sizeof(StratDispatch[0]))
  rc=(StratDispatch[rp->Command](rp));
 else
  rc=(RPERR_COMMAND | RPDONE);
 return rc;
}

#ifdef __cplusplus
}
#endif


