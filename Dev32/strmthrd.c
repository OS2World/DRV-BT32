/* $Id: strmthrd.c,v 1.2 2003/10/01 19:11:20 smilcke Exp $ */

/*
 * strmthrd.c
 * Autor:               Stefan Milcke
 * Erstellt am:         19.08.2003
 * Letzte Aenderung am: 09.09.2003
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
#include <linux/ioctl.h>

#include "stream.h"

extern void CALLSHDENTRYPOINT32(PSHDFN pfn,void* data,int dataLen);
#pragma aux CALLSHDENTRYPOINT32 parm [edx] [ebx] [ecx];

extern volatile int stop_stream_thread[];

//------------------------------ stream_capture1 -------------------------------
static int stream_capture1(struct stream_instance* psi,struct bttv* btv)
{
 struct video_device *video_dev=&(btv->video_dev);
 struct video_mbuf vm;
 struct video_mmap vmap;
 video_dev->ioctl(video_dev,VIDIOCGMBUF,&vm);
 vmap.frame=0;
 vmap.height=psi->DY_Height;
 vmap.width=psi->DX_Width;
 vmap.format=VIDEO_PALETTE_YUV422;
 video_dev->ioctl(video_dev,VIDIOCMCAPTURE,&vmap);
 return vmap.frame;
}

//------------------------------ stream_capture2 -------------------------------
static int stream_capture2(struct stream_instance* psi,struct bttv* btv
                           ,void *pBuffer,int bufLen,int frameNo)

{
 int rc=-1;
 struct video_device *video_dev=&(btv->video_dev);
 struct video_mbuf vm;
 struct video_mmap vmap;
 video_dev->ioctl(video_dev,VIDIOCGMBUF,&vm);
 rc=video_dev->ioctl(video_dev,VIDIOCSYNC,(&(frameNo)));
 if(!rc)
  memcpy(pBuffer,btv->fbuffer+vm.offsets[frameNo],bufLen);
 return rc;
}

//--------------------------------- get_ibuff ----------------------------------
static struct stream_instance_buffer* get_ibuff(struct stream_instance* psi)
{
 struct stream_instance_buffer* pbf=0;
 if(!(list_empty(&(psi->buff_list))))
 {
  struct list_head* lh=psi->buff_list.next;
  pbf=list_entry(lh,struct stream_instance_buffer,node);
  list_del(&pbf->node);
 }
 return pbf;
}

//------------------------------ walk_event_list -------------------------------
static void walk_event_list(struct stream_instance* psi)
{
 struct list_head* lh;
 struct stream_instance_event* pev;
 list_for_each(lh,&(psi->event_list))
 {
  pev=list_entry(lh,struct stream_instance_event,node);
  if(pev)
  {
   if(pev->NTime>psi->ulTime)
   {
    static SHD_REPORTEVENT pe;
    pev->NTime+=pev->Time;
    pe.ulFunction=SHD_REPORT_EVENT;
    pe.hStream=psi->hStream;
    pe.hEvent=pev->hEvent;
    pe.ulStreamTime=pev->NTime-pev->Time;
    CALLSHDENTRYPOINT32(psi->pSHDEntryPoint,&pe,sizeof(SHD_REPORTEVENT));
    lh=psi->event_list.next;
   }
  }
 }
}

//------------------------------- stream_intrpt --------------------------------
static void stream_intrpt(struct stream_instance* psi)
{
 struct bttv* btv=&(bttvs[psi->devNo-1]);
 struct stream_instance_buffer* pbf=0;
 int skipped;
 int overrun;
 int sys_clock;
 int cap_frame;
 int ecx;
 cap_frame=stream_capture1(psi,btv);         // Initiate the capture
 psi->tic_toc++;
 skipped=0;
 sys_clock=getMS();
 ecx=psi->tic_next-sys_clock;
 if(sys_clock>=psi->tic_next)
 {
  if(!(1==psi->ucT_Paused))
   psi->ulTime=sys_clock-psi->ulTime_Delta;
// Stime_Adjusted
  skipped=0;
  overrun=1;
  if(!pbf)
  {
   pbf=get_ibuff(psi);
   if(pbf)
   {
    psi->pCur_Img_Buff=pbf->pBuffer;
    psi->pCur_Img_Free=pbf->pBuffer;
    psi->ulCur_Img_Len=pbf->bufLen;
    psi->ulCur_Img_Size=pbf->bufLen;
   }
  }
  if(pbf)
  {
// Got_a_Buffer
   psi->ulCur_Img_Len-=psi->ulImgSize;
   overrun=0;
   skipped=-1;
   psi->ulFrames++;
  }
// Find_next_time
// next_cap_time
  {
   for(;;)
   {
    int ax=psi->ulCur_Remain+psi->ulOrg_Remain;
    skipped++;
    if(ax>psi->ulDivisor)
    {
     ax-=psi->ulDivisor;
     psi->tic_next--;
    }
    psi->ulCur_Remain=ax;
    psi->tic_next+=psi->ulTic_Delta;
    ecx-=psi->ulTic_Delta;
    if(ecx<0)
     break;
   }
  }
// ReadImage
  // stream_capture2(psi,btv,,,cap_frame);

 }
// Exit_tIntr
 walk_event_list(psi);
}

//------------------------------- stream_thread --------------------------------
int stream_thread(void* arg)
{
 struct stream_instance* psi=(struct stream_instance*)arg;
 struct bttv* btv=&(bttvs[psi->devNo-1]);
 psi->ucIsStreaming++;
 while(0==stop_stream_thread[psi->devNo-1])
 {
  sleep_on_timeout(&btv->capq,100);
  if(0==stop_stream_thread[psi->devNo-1])
   stream_intrpt(psi);
 }
 psi->ucIsStreaming--;
 return 0;
}

/*******************************************************************************/
/* sssssssssss                                                                */
/*******************************************************************************/

//------------------------------- stream_capture -------------------------------
int stream_capture(struct stream_instance* psi,struct bttv* btv
                   ,void* pBuffer,int bufLen)
{
 int rc=-1;
 struct video_device *video_dev=&(btv->video_dev);
 struct video_mbuf vm;
 struct video_mmap vmap;
 video_dev->ioctl(video_dev,VIDIOCGMBUF,&vm);

 vmap.frame=0;
 vmap.height=psi->DY_Height;
 vmap.width=psi->DX_Width;
 vmap.format=VIDEO_PALETTE_YUV422;
 video_dev->ioctl(video_dev,VIDIOCMCAPTURE,&vmap);
 rc=video_dev->ioctl(video_dev,VIDIOCSYNC,(&(vmap.frame)));
 if(!rc)
 {
  int sz=vm.size/vm.frames;
  sz=bufLen;
  memcpy(pBuffer,btv->fbuffer+vm.offsets[vmap.frame],sz);
 }
 return rc;
}

//------------------------------- stream_thread --------------------------------
int stream_thread_old(void* arg)
{
 struct list_head* lh;
 struct stream_instance* psi=(struct stream_instance*)arg;
 struct stream_instance_buffer* pbf=0;
 struct stream_instance_event* pev=0;
 unsigned long devNo=psi->devNo;
 struct bttv* btv=&(bttvs[devNo-1]);
 unsigned long cms;
 unsigned long cur_image_time; // (ECX)
 int skipped;
 int overrun;
 int headersize=sizeof(VIDEO_FRAME_HDR);
 int round;
 psi->ucIsStreaming++;
 while(0==stop_stream_thread[devNo-1])
 {
  sleep_on_timeout(&btv->capq,100);
  if(0==stop_stream_thread[devNo-1])
  {
   cms=getMS();                           // Get current time
   cur_image_time=psi->tic_next-cms;
   if(psi->tic_next<=cms)                 // Next capture time reached?
   {                                      // Yes
    if(!psi->ucT_Paused)                  // Stream paused?
     psi->ulTime=cms-psi->ulTime_Delta;   // No
    skipped=0;                            // Number of frames skipped
    overrun=1;                            // Assume Overrun occurs (no Buffer available)
    if(!pbf)                              // Pointer to capture buffer valid?
    {                                     // No, retrieve one from the list
     if(!(list_empty(&(psi->buff_list)))) // List empty?
     {                                    // No, get the first entry
      lh=psi->buff_list.next;
      pbf=list_entry(lh,struct stream_instance_buffer,node);
      list_del(&pbf->node);               // and remove it from the list
      psi->pCur_Img_Buff=pbf->pBuffer;
      psi->pCur_Img_Free=pbf->pBuffer;
      psi->ulCur_Img_Len=pbf->bufLen;
      psi->ulCur_Img_Size=pbf->bufLen;
     }
    }
    if(pbf)
    {
     skipped=-1;
     overrun=0;
     psi->ulFrames++;
    }
    // Find next time
    round=0;
    for(;;)
    {
     int ax;
     skipped++;
     ax=psi->ulCur_Remain+psi->ulOrg_Remain;
     if(psi->ulDivisor>ax)
      ax-=psi->ulDivisor;
     break;
    }

    if(pbf)
    {
     psi->ulFrames++;
     while(psi->tic_next<=cms)
      psi->tic_next+=psi->ulTic_Delta;
     if(!stream_capture(psi,btv
                        ,(void*)(((int)psi->pCur_Img_Free)+headersize)
                        ,psi->ulImgSize-headersize))
     {
      PVIDEO_FRAME_HDR pvh=(PVIDEO_FRAME_HDR)psi->pCur_Img_Free;
      pvh->FrameNumber=psi->ulFrames;
      pvh->FramesSkipped=0;
      pvh->StreamTime=psi->ulTime;
      pvh->FrameSize=psi->ulImgSize-headersize;
      pvh->reserved1=cms;
      psi->ulMissing=0;
      psi->pCur_Img_Free=(void*)(((int)(psi->pCur_Img_Free))+psi->ulImgSize);
      psi->ulCur_Img_Len-=psi->ulImgSize;
      if(psi->ulCur_Img_Len<psi->ulImgSize)
      { // Report SH, that buffer is full
       static SHD_REPORTINT pa;
       pa.ulFunction=SHD_REPORT_INT;
       pa.hStream=psi->hStream;
       pa.pBuffer=pbf->pBuffer;
       psi->pCur_Img_Buff=0;
       pa.ulFlag=SHD_READ_COMPLETE;
       pa.ulStatus=psi->ulCur_Img_Size-psi->ulCur_Img_Len;
       pa.ulStreamTime=psi->ulTime;
       CALLSHDENTRYPOINT32(psi->pSHDEntryPoint,&pa,sizeof(SHD_REPORTINT));
       kfree(pbf);
       pbf=0;
      }
      // Handle underrun/overrun here
      // End Handle underrun/overrun here

     }
    }
   }
   // Walk Event List
   walk_event_list(psi);
  }
 }
 stop_stream_thread[devNo-1]++;
 psi->ucIsStreaming--;
 return 0;
}

#ifdef __cplusplus
}
#endif
