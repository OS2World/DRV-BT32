/* $Id: ioctl.c,v 1.19 2004/01/29 20:40:39 smilcke Exp $ */

/*
 * ioctl.c
 * Autor:               Stefan Milcke
 * Erstellt am:         12.11.2001
 * Letzte Aenderung am: 14.07.2003
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
#include <ldefos2.h>
#include <bt_ioctl.h>
#include <lxapioctl.h>
#include "Ver_32.h"

#include <bttvos2.h>

#include <linux/types.h>
#include <linux/string.h>
#include <linux/kmod.h>
#include <lxapilib.h>

char vendor[]="Stefan Milcke";

extern int StrmRegisterVSDInstance(int flags,ULONG vsdpid,ULONG devno);

//-------------------------------- ioctl_global --------------------------------
ULONG ioctl_global(RPIOCtl __far *rp)
{
 ULONG rc=0;
 switch(rp->Function)
 {
  case BTIOCFN_GLO_GETDRIVERINFO:
   {
    DATAPACKET(dp,PBTIOCDP_GLO_GETDRIVERINFO,rp,VERIFY_READWRITE);
    if(dp)
    {
     dp->ulVersionMajor=BT32_DRV_MAJOR_VERSION;
     dp->ulVersionMinor=BT32_DRV_MINOR_VERSION;
     dp->ulBuildLevel=BUILD_LEVEL;
     strcpy(dp->vendor,vendor);
     dp->ulNumModules=OS2_get_total_num_modules();
     dp->ulV4LXNumDevices=OS2_v4lx_get_num_devices();
     strcpy(dp->tvname,OS2_bttv_get_name(bttv_num-1));
     strcpy(dp->tunername,OS2_bttv_get_tuner_name(bttv_num-1));
     strcpy(dp->tunertype,OS2_tun_get_tuner_type(OS2_bttv_get_tuner_type(bttv_num-1)));
    }
    else
     rc=RPERR_PARAMETER;
   }
   break;
  case BTIOCFN_GLO_REGISTERVSDINSTANCE:
   {
    PARMPACKET(pa,PBTIOCPA_GLO_RDVSDINSTANCE,rp,VERIFY_READWRITE);
    if(pa)
     pa->rc=StrmRegisterVSDInstance(pa->flags,pa->vsdpid,pa->devno);
    else
     rc=RPERR_PARAMETER;
   }
   break;
  default:
   rc=RPERR_COMMAND;
   break;
 }
 return rc | RPDONE;
}

//--------------------------------- ioctl_btv ----------------------------------
ULONG ioctl_btv(RPIOCtl __far *rp)
{
 ULONG rc=0;
 switch(rp->Function)
 {
  case BTIOCFN_BTV_GETEEPROM:
   {
    DATAPACKET(p,PBTIOCDP_BTV_GETEEPROM,rp,VERIFY_READWRITE);
    if(p)
    {
     p->rc=0;
     OS2_bttv_get_eeprom(p->eeprom);
    }
    else
     rc=RPERR_PARAMETER;
   }
   break;
  default:
   rc=RPERR_COMMAND;
   break;
 }
 return rc | RPDONE;
}

//--------------------------------- StratIOCtl ---------------------------------
ULONG StratIOCtl(RPIOCtl __far* rp);
#pragma aux StratIOCtl parm [fs ebx] modify [esi edi eax ebx ecx edx];
ULONG StratIOCtl(RPIOCtl __far* rp)
{
 switch(rp->Category)
 {
  case BTIOCCAT_GLO:
   return ioctl_global(rp);
  case BTIOCCAT_BTV:
   return ioctl_btv(rp);
  default:
   return OS2_lxioctl((void __far*)rp);
 }
}

#ifdef __cplusplus
}
#endif
