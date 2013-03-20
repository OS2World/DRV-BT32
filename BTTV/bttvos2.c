/* $Id: bttvos2.c,v 1.11 2004/01/29 09:10:13 smilcke Exp $ */

/*
 * bttvos2.c
 * Autor:               Stefan Milcke
 * Erstellt am:         14.11.2001
 * Letzte Aenderung am: 29.01.2004
 *
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <linux/kernel.h>
//#include <linux/major.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/ioctl.h>
#include <linux/timer.h>
#ifndef FAR
#define FAR_LDEFOS2
#endif
#include <ldefos2.h>
#ifdef FAR_LDEFOS2
#undef FAR
#undef FAR_LDEFOS2
#endif

#include "bt848.h"
#include "bttv.h"
#include "bttvp.h"

static char NONE[]="None";

extern int bttv_num;

/******************************************************************************/
/* Some API's to call from OS/2 device driver                                 */
/******************************************************************************/
//----------------------------- OS2_bttv_get_name ------------------------------
char *OS2_bttv_get_name(int nr)
{
 if(nr<bttv_num)
  if(bttvs[nr].video_dev.name)
   return bttvs[nr].video_dev.name;
 return NONE;
}

//-------------------------- OS2_bttv_get_tuner_name ---------------------------
char *OS2_bttv_get_tuner_name(int nr)
{
 if(nr<bttv_num)
  if(bttvs[nr].tuner_name)
   return bttvs[nr].tuner_name;
 return NONE;
}

extern unsigned char eeprom_data[256];
//---------------------------- OS2_bttv_get_eeprom -----------------------------
void OS2_bttv_get_eeprom(unsigned char *data)
{
 memcpy(data,eeprom_data,256);
}

//-------------------------- OS2_bttv_get_tuner_type ---------------------------
int OS2_bttv_get_tuner_type(int nr)
{
 if(nr<bttv_num)
  return bttvs[nr].tuner_type;
 return -1;
}

#ifdef __cplusplus
}
#endif
