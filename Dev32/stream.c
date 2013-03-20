/* $Id: stream.c,v 1.3 2003/08/03 21:16:45 smilcke Exp $ */

/*
 * stream.c
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
#define INCL_DOSERRORS
#include <os2.h>
#include <os2me.h>
#include <shdd.h>

#include <devhlp32.h>
#include <devtype.h>
#include <devrp.h>
#include "devown.h"

#include <lxapilib.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/semaphor.h>

#include "stream.h"

extern WORD32 StrmIOCtl(int devNo,RP __far* rp);

typedef struct vsd_instance
{
 struct list_head node;
 ULONG vsdpid;
 ULONG devno;
} VSD_INSTANCE;


LIST_HEAD(open_instances);
LIST_HEAD(vsd_instances);
static spinlock_t open_instances_lock=SPIN_LOCK_UNLOCKED;

static char devFileName[]="bt32.sys";

//----------------------------- StrmGetVSDInstance -----------------------------
struct vsd_instance* StrmGetVSDInstance(ULONG vsdpid,ULONG devno)
{
 struct list_head* lh;
 struct vsd_instance* p=0;
 list_for_each(lh,&vsd_instances)
 {
  p=list_entry(lh,struct vsd_instance,node);
  if(p->vsdpid==vsdpid && p->devno==devno)
   break;
  p=0;
 }
 return p;
}

//-------------------------- StrmRegisterVSDInstance ---------------------------
int StrmRegisterVSDInstance(int flags,ULONG vsdpid,ULONG devno)
{
 int rc=0;
 struct vsd_instance* p;
 p=StrmGetVSDInstance(vsdpid,devno);
 if(flags==1)
 {
  if(!p)
  {
   p=(struct vsd_instance*)kmalloc(sizeof(struct vsd_instance),0);
   if(p)
   {
    p->vsdpid=vsdpid;
    p->devno=devno;
    list_add(&p->node,&vsd_instances);
   }
   else
    rc=-ENOMEM;
  }
 }
 else
 {
  if(p)
  {
   list_del(&p->node);
   kfree(p);
  }
 }
 return rc;
}

//-------------------------------- Strm_Add_OI ---------------------------------
struct open_instance* Strm_Add_OI(ULONG ulFileID)
{
 ULONG f;
 struct open_instance* p=(struct open_instance*)kmalloc(sizeof(struct open_instance),0);
 if(p)
 {
  memset(p,0,sizeof(struct open_instance));
  INIT_LIST_HEAD(&(p->OI_SI));
  p->OI_FileNum=ulFileID;
  f=DevPushfCli();
  list_add(&p->node,&open_instances);
  DevPopf(f);
 }
 return p;
}

//-------------------------------- Strm_Find_OI --------------------------------
struct open_instance* Strm_Find_OI(ULONG ulFileID)
{
 struct open_instance* p=0;
 struct list_head* lh;
 list_for_each(lh,&open_instances)
 {
  p=list_entry(lh,struct open_instance,node);
  if(p->OI_FileNum==ulFileID)
   return p;
 }
 return 0;
}

//------------------------------- Strm_Remove_OI -------------------------------
int Strm_Remove_OI(ULONG ulFileID)
{
 ULONG f;
 int rc=1;
 struct open_instance* p;
 p=Strm_Find_OI(ulFileID);
 if(p)
 {
  while(!(list_empty(&(p->OI_SI))))
  {
   struct list_head* lh=p->OI_SI.next;
   struct stream_instance* psi=list_entry(lh,struct stream_instance,node);
   if(psi->hStream)
   {
    {
     DDCMDCONTROL control={0};
     control.ulFunction=DDCMD_CONTROL;
     control.hStream=psi->hStream;
     control.ulCmd=DDCMD_STOP;
     StrmControl(&control);
    }
    {
     DDCMDDEREGISTER dereg={0};
     dereg.ulFunction=DDCMD_DEREG_STREAM;
     dereg.hStream=psi->hStream;
     StrmDeregStream(&dereg);
    }
   }
//   Strm_Remove_SI(ulFileID,psi->hStream);
  }
  f=DevPushfCli();
  list_del(&p->node);
  DevPopf(f);
  kfree(p);
  rc=0;
 }
 return rc;
}

//-------------------------------- Strm_Add_SI ---------------------------------
struct stream_instance* Strm_Add_SI(ULONG ulFileID,HSTREAM hStream)
{
 ULONG f;
 struct open_instance* pOI;
 struct stream_instance* p=0;
 pOI=Strm_Find_OI(ulFileID);
 if(pOI)
 {
  p=(struct stream_instance*)kmalloc(sizeof(struct stream_instance),0);
  if(p)
  {
   memset(p,0,sizeof(struct stream_instance));
   p->hStream=hStream;
   p->ulSysFileNum=ulFileID;
   p->devNo=pOI->devNo;
   INIT_LIST_HEAD(&(p->buff_list));
   INIT_LIST_HEAD(&(p->event_list));
   f=DevPushfCli();
   list_add(&p->node,&(pOI->OI_SI));
   DevPopf(f);
  }
 }
 return p;
}

//-------------------------------- Strm_Find_SI --------------------------------
struct stream_instance* Strm_Find_SI(ULONG ulFileID,HSTREAM hStream)
{
 struct stream_instance* p=0;
 struct open_instance* poi;
 struct list_head* lh;
 if(ulFileID)
 {
  poi=Strm_Find_OI(ulFileID);
  if(poi)
  {
   list_for_each(lh,&(poi->OI_SI))
   {
    p=list_entry(lh,struct stream_instance,node);
    if(p->hStream==hStream)
     return p;
   }
  }
 }
 else
 {
  struct list_head* lhoi;
  list_for_each(lhoi,&open_instances)
  {
   poi=list_entry(lhoi,struct open_instance,node);
   list_for_each(lh,&(poi->OI_SI))
   {
    p=list_entry(lh,struct stream_instance,node);
    if(p->hStream==hStream)
     return p;
   }
  }
 }
 return 0;
}

//------------------------------- Strm_Remove_SI -------------------------------
ULONG Strm_Remove_SI(ULONG ulFileID,HSTREAM hStream)
{
 ULONG f;
 ULONG rc=1;
 struct stream_instance* p=Strm_Find_SI(ulFileID,hStream);
 if(p)
 {
  while(!(list_empty(&(p->buff_list))))
  {
   struct list_head* lh=p->buff_list.next;
   struct stream_instance_buffer* pbf=list_entry(lh,struct stream_instance_buffer,node);
   list_del(&pbf->node);
   kfree(pbf);
  }
  while(!(list_empty(&(p->event_list))))
  {
   struct list_head* lh=p->event_list.next;
   struct stream_instance_event* pev=list_entry(lh,struct stream_instance_event,node);
   Strm_Remove_EV(p,pev->hEvent);
  }
  f=DevPushfCli();
  list_del(&p->node);
  DevPopf(f);
  kfree(p);
  rc=0;
 }
 return rc;
}

//-------------------------------- Strm_Add_EV ---------------------------------
struct stream_instance_event* Strm_Add_EV(struct stream_instance* psi,HEVENT hEvent)
{
 ULONG f;
 struct stream_instance_event* p=Strm_Find_EV(psi,hEvent);
 if(!p)
 {
  p=(struct stream_instance_event*)kmalloc(sizeof(stream_instance_event),0);
  memset(p,0,sizeof(stream_instance_event));
  p->hEvent=hEvent;
  f=DevPushfCli();
  list_add(&p->node,&psi->event_list);
  DevPopf(f);
  return p;
 }
 return 0;
}

//-------------------------------- Strm_Find_EV --------------------------------
struct stream_instance_event* Strm_Find_EV(struct stream_instance* psi,HEVENT hEvent)
{
 struct list_head* lh;
 list_for_each(lh,&(psi->event_list))
 {
  struct stream_instance_event* p=list_entry(lh,struct stream_instance_event,node);
  if(p->hEvent==hEvent)
   return p;
 }
 return 0;
}

//------------------------------- Strm_Remove_EV -------------------------------
ULONG Strm_Remove_EV(struct stream_instance* psi,HEVENT hEvent)
{
 ULONG f;
 struct stream_instance_event* p=Strm_Find_EV(psi,hEvent);
 if(p)
 {
  f=DevPushfCli();
  list_del(&p->node);
  DevPopf(f);
  kfree(p);
  return 0;
 }
 return 1;
}

//---------------------------------- StrmOpen ----------------------------------
#pragma off (unreferenced)
WORD32 StrmOpen(int devNo,RP __far* rp)
#pragma on (unreferenced)
{
 WORD32 rc=RPDONE;
 ULONG FileID=(ULONG)(((RPOpenClose __far*)(rp))->FileID);
 struct vsd_instance* pvsd=0;
 struct open_instance* poi=0;
 unsigned long f;
 spin_lock_irqsave(&open_instances_lock,f);
 pvsd=StrmGetVSDInstance(OS2_get_current_pid(),devNo);
 if(!pvsd)
  rc|=(RPERR_GENERAL | RPBUSY);
 else
 {
  poi=Strm_Add_OI(FileID);
  if(poi)
  {
   poi->devNo=devNo;
   poi->btv=&(bttvs[devNo-1]);
   OS2_open_device_called((USHORT)FileID,devFileName);
  }
  else
   rc|=RPERR_GENERAL;
 }
 spin_unlock_irqrestore(&open_instances_lock,f);
 return rc;
}

//--------------------------------- StrmClose ----------------------------------
#pragma off (unreferenced)
WORD32 StrmClose(int devNo,RP __far* rp)
#pragma on (unreferenced)
{
 ULONG FileID=(ULONG)(((RPOpenClose __far*)(rp))->FileID);
 Strm_Remove_OI(FileID);
 OS2_close_device_called((USHORT)FileID);
 return RPDONE;
}

//---------------------------------- StrmInit ----------------------------------
#pragma off (unreferenced)
WORD32 StrmInit(int devNo,RP __far* rp)
#pragma on (unreferenced)
{
 return RPDONE;
}

//------------------------------ StrmInitComplete ------------------------------
#pragma off (unreferenced)
WORD32 StrmInitComplete(int devNo,RP __far* rp)
#pragma on (unreferenced)
{
 return RPDONE;
}

//-------------------------------- StrmShutdown --------------------------------
#pragma off (unreferenced)
WORD32 StrmShutdown(int devNo,RP __far* rp)
#pragma on (unreferenced)
{
 return RPDONE;
}

//--------------------------------- StrmError ----------------------------------
#pragma off (unreferenced)
static WORD32 StrmError(int devNo,RP __far* rp)
#pragma on (unreferenced)
{
 return RPERR_COMMAND | RPDONE;
}

//------------------------------- StrmDeinstall --------------------------------
#pragma off (unreferenced)
static WORD32 StrmDeinstall(int devNo,RP __far* rp)
#pragma on (unreferenced)
{
 return RPDONE;
}

typedef WORD32 (*RPHandler)(int devNo,RP __far* rp);
RPHandler StrmDispatch[] =
{
  StrmInit,                  // 00 (BC): Initialization
  StrmError,                 // 01 (B ): Media check
  StrmError,                 // 02 (B ): Build BIOS parameter block
  StrmError,                 // 03 (  ): Unused
  StrmError,                 // 04 (BC): Read
  StrmError,                 // 05 ( C): Nondestructive read with no wait
  StrmError,                 // 06 ( C): Input status
  StrmError,                 // 07 ( C): Input flush
  StrmError,                 // 08 (BC): Write
  StrmError,                 // 09 (BC): Write verify
  StrmError,                 // 0A ( C): Output status
  StrmError,                 // 0B ( C): Output flush
  StrmError,                 // 0C (  ): Unused
  StrmOpen,                  // 0D (BC): Open
  StrmClose,                 // 0E (BC): Close
  StrmError,                 // 0F (B ): Removable media check
  StrmIOCtl,                 // 10 (BC): IO Control
  StrmError,                 // 11 (B ): Reset media
  StrmError,                 // 12 (B ): Get logical unit
  StrmError,                 // 13 (B ): Set logical unit
  StrmDeinstall,             // 14 ( C): Deinstall character device driver
  StrmError,                 // 15 (  ): Unused
  StrmError,                 // 16 (B ): Count partitionable fixed disks
  StrmError,                 // 17 (B ): Get logical unit mapping of fixed disk
  StrmError,                 // 18 (  ): Unused
  StrmError,                 // 19 (  ): Unused
  StrmError,                 // 1A (  ): Unused
  StrmError,                 // 1B (  ): Unused
  StrmShutdown,              // 1C (BC): Notify start or end of system shutdown
  StrmError,                 // 1D (B ): Get driver capabilities
  StrmError,                 // 1E (  ): Unused
  StrmInitComplete           // 1F (BC): Notify end of initialization
};

//---------------------------------- Strategy ----------------------------------
WORD32 StreamStrategy(int devNo, RP __far* rp);
#pragma aux (STRATEGY) StreamStrategy "STREAM_STRATEGY_ENTRY" parm [ecx] [fs ebx];
WORD32 StreamStrategy(int devNo, RP __far* rp)
{
 WORD32 rc=RPDONE;
 if(rp->Command<sizeof(StrmDispatch)/sizeof(StrmDispatch[0]))
  rc=(StrmDispatch[rp->Command](devNo,rp));
 else
  rc=RPERR_COMMAND | RPDONE;
 return rc;
}

#ifdef __cplusplus
}
#endif


