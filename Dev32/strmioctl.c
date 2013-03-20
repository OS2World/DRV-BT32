/* $Id: strmioctl.c,v 1.3 2003/08/03 21:16:47 smilcke Exp $ */

/*
 * strmioctl.c
 * Autor:               Stefan Milcke
 * Erstellt am:         28.06.2003
 * Letzte Aenderung am: 26.07.2003
 *
*/

#ifdef __cplusplus
extern "C" {
#endif

#define INCL_NOPMAPI
#define INCL_DOSINFOSEG
#include <os2.h>
#include <os2me.h>
#include <shdd.h>

#include <devhlp32.h>
#include <devtype.h>
#include <devrp.h>
#include "devown.h"

#include <lxapilib.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kmod.h>

#include "stream.h"

typedef VCASCREENINFO      *NPVCASCREENINFO;
typedef VCASETCAPTURERECT  *NPVCASETCAPTURERECT;
typedef VCASETFPS          *NPVCASETFPS;
typedef VCACAPISIZE        *NPVCACAPISIZE;

//--------------------------------- StrmIOCtl ----------------------------------
#pragma off (unreferenced)
WORD32 StrmIOCtl(int devNo,RPIOCtl __far* rp)
#pragma on (unreferenced)
{
 ULONG category=rp->Category;
 ULONG function=rp->Function;
 if(category==VIDEO_IOCTL_CAT)
 {
  struct open_instance* poi=Strm_Find_OI(rp->FileID);
  switch(function)
  {
   case VCAI_SCREENINFO:                        // CAT 140 FN 7C
    {
     DATAPACKET(dp,NPVCASCREENINFO,rp,VERIFY_READWRITE);
     if(dp)
     {
      memcpy(&poi->ScreenInfo,dp,dp->ulLength);
      return RPDONE;
     }
     else
      return RPDONE | RPERR_PARAMETER;
    }
   case VCAI_SETCAPTRECT:                       // CAT 140 FN 6B
    {
     DATAPACKET(dp,NPVCASETCAPTURERECT,rp,VERIFY_READWRITE);
     if(dp)
     {
      memcpy(&poi->CaptureRect,dp,sizeof(VCASETCAPTURERECT));
      return RPDONE;
     }
     else
      return RPDONE | RPERR_PARAMETER;
    }
   case VCAI_SETFPS:                            // CAT 140 FN 76
    {
     DATAPACKET(dp,NPVCASETFPS,rp,VERIFY_READWRITE);
     if(dp)
     {
      memcpy(&poi->FPS,dp,sizeof(VCASETFPS));
      return RPDONE;
     }
     else
      return RPDONE | RPERR_PARAMETER;
    }
   case VCAI_VALIDRECT:                         // CAT 140 FN 6E
    {
     DATAPACKET(dp,NPVCACAPISIZE,rp,VERIFY_READWRITE);
     if(dp)
     {
      memcpy(&poi->CapISize,dp,sizeof(VCACAPISIZE));
     }
     else
      return RPDONE | RPERR_PARAMETER;
    }
  }
 }
 return OS2_lxioctl((void __far*)rp);
}

#ifdef __cplusplus
};
#endif
