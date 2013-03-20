/* $Id: bt_ioctl.h,v 1.10 2003/07/22 22:05:12 smilcke Exp $ */

/*
 * bt_ioctl.h
 * Autor:               Stefan Milcke
 * Erstellt am:         12.11.2001
 * Letzte Aenderung am: 15.07.2003
 *
*/

#ifndef BT_IOCTL_H_INCLUDED
#define BT_IOCTL_H_INCLUDED

#pragma pack(1)

#define BTIOCCAT_GLO       0x80

#define BTIOCCAT_BTV       0x81

/******************************************************************************/
/* global ioctl functions                                                     */
/******************************************************************************/
//--------------------------- global get driver info ---------------------------
#define BTIOCFN_GLO_GETDRIVERINFO                           0x01
typedef struct _BTIOCDP_GLO_GETDRIVERINFO
{
 ULONG ulVersionMajor;
 ULONG ulVersionMinor;
 ULONG ulBuildLevel;
 CHAR vendor[32];
 ULONG ulNumModules;
 ULONG ulV4LXNumDevices;
 CHAR tvname[64];
 CHAR tunername[64];
 CHAR tunertype[64];
} BTIOCDP_GLO_GETDRIVERINFO,*PBTIOCDP_GLO_GETDRIVERINFO;

#define BTIOCFN_GLO_REGISTERVSDINSTANCE                     0x02
typedef struct _BTIOCPA_GLO_RDVSDINSTANCE
{
 INT rc;
 INT flags;        // 1 = Register, 0 = deregister
 ULONG vsdpid;
 ULONG devno;
} BTIOCPA_GLO_RDVSDINSTANCE,*PBTIOCPA_GLO_RDVSDINSTANCE;

#define BTIOCFN_



/******************************************************************************/
/* Special ioctls for bttv                                                    */
/******************************************************************************/
#define BTIOCFN_BTV_GETEEPROM                               0x01
typedef struct _BTIOCDP_BTV_GETEEPROM
{
 INT rc;
 UCHAR eeprom[256];
}BTIOCDP_BTV_GETEEPROM,*PBTIOCDP_BTV_GETEEPROM;

#pragma pack()
#endif //BT_IOCTL_H_INCLUDED
