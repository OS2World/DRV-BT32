/* $Id: idc.c,v 1.12 2004/02/16 21:24:58 smilcke Exp $ */

/*
 * idc.c
 * Autor:               Stefan Milcke
 * Erstellt am:         08.11.2001
 * Letzte Aenderung am: 04.03.2003
 *
*/
#ifdef __cplusplus
extern "C" {
#endif

#define INCL_NOPMAPI
#define INCL_DOSINFOSEG
#include <os2.h>
#include <devtype.h>
#include <devhlp32.h>
//#include <strategy.h>
#include <stacktoflat.h>
#include <linux/types.h>

//packet pointer must reference a structure on the stack

WORD32 IDCEntry(ULONG cmd, ULONG packet);
#pragma aux IDCEntry "IDC_ENTRY" parm reverse [ecx edx]
WORD32 IDCEntry(ULONG cmd, ULONG packet)
{
 return 0;
}

#ifdef __cplusplus
}
#endif

