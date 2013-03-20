/* $Id: misc.c,v 1.10 2003/08/03 21:16:48 smilcke Exp $ */

/*
 * misc.c
 * Autor:               Stefan Milcke
 * Erstellt am:         08.11.2001
 * Letzte Aenderung am: 28.07.2003
 *
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "hwaccess.h"
#include <linux/init.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <asm/hardirq.h>
#include <linux/ioport.h>
#include <linux/utsname.h>
#include <linux/time.h>

//----------------------------------- printk -----------------------------------
int printk(const char * fmt, ...)
{
  return 0;
}

//--------------------------------- poll_wait ----------------------------------
void poll_wait(struct file * filp, wait_queue_head_t * wait_address, poll_table *p)
{

}

void iodelay32(unsigned long);
// Wrong parameter register [eax] ??? (SM)
//#pragma aux iodelay32 parm nomemory [eax] modify nomemory exact [eax ecx];
#pragma aux iodelay32 parm nomemory [ecx] modify nomemory exact [eax ecx];

//---------------------------------- __udelay ----------------------------------
void __udelay(unsigned long usecs)
{
  iodelay32(usecs*2);
}

#ifdef __cplusplus
}
#endif
