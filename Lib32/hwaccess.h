/* $Id: hwaccess.h,v 1.8 2003/03/03 23:52:26 smilcke Exp $ */

/*
 **********************************************************************
 *     hwaccess.h
 *     Copyright 1999, 2000 Creative Labs, Inc.
 *
 **********************************************************************
 *
 *     Date		    Author	    Summary of changes
 *     ----		    ------	    ------------------
 *     October 20, 1999     Bertrand Lee    base code release
 *
 **********************************************************************
 *
 *     This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation; either version 2 of
 *     the License, or (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public
 *     License along with this program; if not, write to the Free
 *     Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
 *     USA.
 *
 **********************************************************************
 */

#ifndef _HWACCESS_H
#define _HWACCESS_H

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/malloc.h>
#include <linux/pci.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <asm/dma.h>

enum GlobalErrorCode
{
	CTSTATUS_SUCCESS = 0x0000,
	CTSTATUS_ERROR,
	CTSTATUS_NOMEMORY,
	CTSTATUS_INUSE,
};

#define FLAGS_AVAILABLE     0x0001
#define FLAGS_READY         0x0002

#define min(x,y) ((x) < (y)) ? (x) : (y)

struct memhandle
{
	unsigned long busaddx;
	void *virtaddx;
	u32 order;
};

#define DEBUG_LEVEL 2

#ifdef TARGET_OS2
#ifdef DEBUG
#ifdef __cplusplus
extern "C" {
#endif
void _cdecl DPD(int level, char *x, ...) ; /* not debugging: nothing */
#ifdef __cplusplus
}
#endif

#define ERROR()	_asm int 3
#else
#define DPD 1 ? (void)0 : (void)((int (*)(int, char *, ...)) NULL)
#define ERROR()
#endif
#define DPF(level, x)
#define __attribute(a)
#else
#ifdef EMU10K1_DEBUG
# define DPD(level,x,y...) do {if(level <= DEBUG_LEVEL) printk( KERN_NOTICE "emu10k1: %s: %d: " x , __FILE__ , __LINE__ , y );} while(0)
# define DPF(level,x)   do {if(level <= DEBUG_LEVEL) printk( KERN_NOTICE "emu10k1: %s: %d: " x , __FILE__ , __LINE__ );} while(0)
#define ERROR() DPF(1,"error\n");
#else
# define DPD(level,x,y...) /* not debugging: nothing */
# define DPF(level,x)
#define ERROR()
#endif /* EMU10K1_DEBUG */
#endif //TARGET_OS2

#endif  /* _HWACCESS_H */
