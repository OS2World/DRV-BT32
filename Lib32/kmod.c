/* $Id: kmod.c,v 1.9 2003/03/03 23:52:26 smilcke Exp $ */

/*
 * kmod.c
 * Autor:               Stefan Milcke
 * Erstellt am:         31.10.2001
 * Letzte Aenderung am: 03.03.2003
 *
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <linux/wait.h>
#include <asm/semaphor.h>

//static DECLARE_MUTEX(dev_probe_sem);
static struct semaphore dev_probe_sem;

void dev_probe_lock(void)
{
 down(&dev_probe_sem);
}

void dev_probe_unlock(void)
{
 up(&dev_probe_sem);
}

#ifdef __cplusplus
}
#endif
