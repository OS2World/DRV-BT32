/* $Id: strmcmd.c,v 1.2 2003/09/03 21:03:38 smilcke Exp $ */

/*
 * strmcmd.c
 * Autor:               Stefan Milcke
 * Erstellt am:         24.07.2003
 * Letzte Aenderung am: 03.09.2003
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

volatile int stop_stream_thread[BTTV_MAX]={0};

ULONG StrmCtrlStart(DDCMDCONTROL* packet,stream_instance* psi);
ULONG StrmCtrlStop(DDCMDCONTROL* packet,stream_instance* psi);
int stream_thread(void* arg);

//----------------------------- StrmStartStreaming -----------------------------
ULONG StrmStartStreaming(struct stream_instance* psi)
{
 unsigned long devNo=psi->devNo;
 stop_stream_thread[devNo-1]=0;
 kernel_thread(stream_thread,(void*)psi,0);
 return 0;
}

//----------------------------- StrmStopStreaming ------------------------------
ULONG StrmStopStreaming(struct stream_instance* psi)
{
 unsigned long devNo=psi->devNo;
 stop_stream_thread[devNo-1]=1;
 schedule();
 return 0;
}

//------------------------------- StrmCtrlStart --------------------------------
ULONG StrmCtrlStart(DDCMDCONTROL* packet,stream_instance* psi)
{
 ULONG rc=-1;
 if(!psi->ucIsStreaming)
 {
  ULONG t_EAX=-1;
  ULONG t_EBX=-1;
  psi->ulFrames=0;
  psi->ulSkipped=0;
  psi->ulMissing=0;
  psi->ulOverrun=0;
  psi->ulUnderrun=0;
  psi->tic_next=getMS();
  psi->ulSTime_Base=psi->tic_next;
  if(0==psi->ucT_Paused)
   psi->ulTime_Delta=psi->ulSHTime;
  psi->ulSTime=psi->tic_next;
  if(0==psi->ulFPSFlags)
  {
   if(psi->ulFPS)
   {
    t_EBX=psi->ulFPS;
    psi->ulDivisor=t_EBX;
    t_EAX=1000;
   }
  }
  else
  {
   t_EBX=1000;
   psi->ulDivisor=t_EBX;
   t_EAX=psi->ulFPS;
  }
  if(t_EBX>0 && -1!=t_EBX)
  {
   ULONG q=t_EAX/t_EBX;
   ULONG r=t_EAX%t_EBX;
   psi->ulTic_Delta=q;
   psi->ulCur_Remain=0;
   psi->ulOrg_Remain=r;
   psi->tic_next+=psi->ulTic_Delta;
   StrmStartStreaming(psi);
   rc=0;
  }
 }
 else
  rc=StrmCtrlStop(packet,psi);
 return rc;
}

//-------------------------------- StrmCtrlStop --------------------------------
ULONG StrmCtrlStop(DDCMDCONTROL* packet,stream_instance* psi)
{
 ULONG f=DevPushfCli();
 ULONG rc=-1;
 StrmStopStreaming(psi);
 DevPopf(f);
 rc=0;
 return rc;
}

//------------------------------- StrmCtrlPause --------------------------------
ULONG StrmCtrlPause(DDCMDCONTROL* packet,stream_instance* psi)
{
 ULONG rc=-1;
 DebugInt3();
 return rc;
}

//------------------------------- StrmCtrlResume -------------------------------
ULONG StrmCtrlResume(DDCMDCONTROL* packet,stream_instance* psi)
{
 ULONG rc=-1;
 DebugInt3();
 return rc;
}

//---------------------------- StrmCtrlEnableEvent -----------------------------
ULONG StrmCtrlEnableEvent(DDCMDCONTROL* packet,stream_instance* psi)
{
 ULONG f=DevPushfCli();
 ULONG rc=-1;
 struct stream_instance_event* pev=Strm_Find_EV(psi,packet->hEvent);
 if(!pev)
 {
  pev=Strm_Add_EV(psi,packet->hEvent);
  if(pev)
  {
   CONTROL_PARM* pctl;
   DevVirtToLin(SELECTOROF(packet->pParm),OFFSETOF(packet->pParm),(LINEAR _far*)&pctl);
   pev->Time=pctl->ulTime;
   pev->NTime=psi->ulTime+getMS();
   rc=0;
  }
 }
 else
  rc=-2; // Event already in queue
 DevPopf(f);
 return rc;
}

//---------------------------- StrmCtrlDisableEvent ----------------------------
ULONG StrmCtrlDisableEvent(DDCMDCONTROL* packet,stream_instance* psi)
{
 ULONG f=DevPushfCli();
 ULONG rc=-1;
 if(0==Strm_Remove_EV(psi,packet->hEvent))
  rc=0;
 DevPopf(f);
 return rc;
}

//----------------------------- StrmCtrlPauseTime ------------------------------
ULONG StrmCtrlPauseTime(DDCMDCONTROL* packet,stream_instance* psi)
{
 ULONG rc=-1;
 DebugInt3();
 return rc;
}

//----------------------------- StrmCtrlResumeTime -----------------------------
ULONG StrmCtrlResumeTime(DDCMDCONTROL* packet,stream_instance* psi)
{
 ULONG rc=-1;
 DebugInt3();
 return rc;
}

//-------------------------------- StrmControl ---------------------------------
ULONG StrmControl(DDCMDCONTROL* packet)
{
 ULONG rc=-1;
 struct stream_instance* psi=Strm_Find_SI(0,packet->hStream);
 if(psi)
 {
  switch(packet->ulCmd)
  {
   case DDCMD_START:
    rc=StrmCtrlStart(packet,psi);
    break;
   case DDCMD_STOP:
    rc=StrmCtrlStop(packet,psi);
    break;
   case DDCMD_PAUSE:
    rc=StrmCtrlPause(packet,psi);
    break;
   case DDCMD_RESUME:
    rc=StrmCtrlResume(packet,psi);
    break;
   case DDCMD_ENABLE_EVENT:
    rc=StrmCtrlEnableEvent(packet,psi);
    break;
   case DDCMD_DISABLE_EVENT:
    rc=StrmCtrlDisableEvent(packet,psi);
    break;
   case DDCMD_PAUSE_TIME:
    rc=StrmCtrlPauseTime(packet,psi);
    break;
   case DDCMD_RESUME_TIME:
    rc=StrmCtrlResumeTime(packet,psi);
    break;
   default:
    rc=ERROR_INVALID_FUNCTION;
  }
 }
 return rc;
}

#ifdef __cplusplus
}
#endif
