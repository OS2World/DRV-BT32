/* $Id: stream.h,v 1.4 2003/09/03 21:03:38 smilcke Exp $ */

/*
 * stream.h
 * Autor:               Stefan Milcke
 * Erstellt am:         08.07.2003
 * Letzte Aenderung am: 03.09.2003
 *
*/

#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <lxapilib.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/semaphor.h>
#include <linux/process.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/list.h>

#include <../bttv/bttvp.h>

#include <vidvci.h>

#define SIE_Num_Events  (30)

typedef struct open_instance
{
 struct list_head       node;
 ULONG                  OI_FileNum;
 struct list_head       OI_SI;
 ULONG                  OI_hStream;
 ULONG                  devNo;
 struct bttv*           btv;
 VCASCREENINFO          ScreenInfo;
 VCASETCAPTURERECT      CaptureRect;
 VCACAPISIZE            CapISize;
 VCASETFPS              FPS;
 ULONG Reserved[40];
} OPEN_INSTANCE;

typedef struct _VIDEO_FRAME_HDR
{
 ULONG FrameNumber;
 ULONG FramesSkipped;
 ULONG StreamTime;
 ULONG FrameSize;
 ULONG SHparm1;
 ULONG reserved3;
 ULONG reserved2;
 ULONG reserved1;
}VIDEO_FRAME_HDR,*PVIDEO_FRAME_HDR;

typedef struct stream_image_free_buff_hdr
{
 struct  stream_image_free_buff_hdr* next;
 ULONG   Len;
 ULONG   Reserved;
} STREAM_IMAGE_FREE_BUFF_HDR;

typedef struct stream_instance_event
{
 struct  list_head node;
 ULONG   NTime;
 ULONG   Time;
 ULONG   hEvent;
 ULONG   Reserved;
} STREAM_INSTANCE_EVENT;

typedef struct stream_instance_buffer
{
 struct  list_head node;
 ULONG   bufLen;
 PVOID   pBuffer;
} STREAM_INSTANCE_BUFFER;

typedef struct stream_instance
{
 struct list_head node;
 unsigned long ulSysFileNum;
 ULONG   devNo;
 HSTREAM hStream;
 PSHDFN  pSHDEntryPoint;

 ULONG   ulImgSize;
 ULONG   ulFPS;
 ULONG   ulFPSFlags;
 ULONG   ulTime;
 UCHAR   ucStopping;
 ULONG   ulSTime;
 ULONG   ulCTime;
 ULONG   ulFrames;
 ULONG   ulSkipped;
 ULONG   ulMissing;
 ULONG   ulOverrun;
 ULONG   ulUnderrun;
 PVOID   pCur_Img_Buff;
 PVOID   pCur_Img_Free;
 ULONG   ulCur_Img_Len;
 ULONG   ulCur_Img_Size;
 ULONG   ulCur_Remain;
 ULONG   ulOrg_Remain;
 ULONG   ulDivisor;
 ULONG   ScaleFactor;
 ULONG   X_Left;
 ULONG   Y_Top;
 ULONG   Y_Height;
 ULONG   X_Width;
 ULONG   DX_Left;
 ULONG   DY_Top;
 ULONG   DY_Height;
 ULONG   DX_Width;
 struct  list_head buff_list;
 struct  list_head event_list;
 ULONG   ulSHTime;
 ULONG   ulSTime_Base;
 ULONG   ulTime_Delta;
 ULONG   ulTic_Delta;
 ULONG   ulPause_Time;
 ULONG   ulResume_Time;
 ULONG   ulResume_Tic_Delta;

 UCHAR   ucT_Paused;
 UCHAR   ucIsStreaming;

 ULONG   tic_next;
 ULONG   tic_toc;
} STREAM_INSTANCE;

struct open_instance* Strm_Add_OI(ULONG ulFileID);
struct open_instance* Strm_Find_OI(ULONG ulFileID);
int Strm_Remove_OI(ULONG ulFileID);

struct stream_instance* Strm_Add_SI(ULONG ulFileID,HSTREAM hStream);
struct stream_instance* Strm_Find_SI(ULONG ulFileID,HSTREAM hStream);
ULONG Strm_Remove_SI(ULONG ulFileID,HSTREAM hStream);

struct stream_instance_event* Strm_Add_EV(struct stream_instance* psi,HEVENT hEvent);
struct stream_instance_event* Strm_Find_EV(struct stream_instance* psi,HEVENT hEvent);
ULONG Strm_Remove_EV(struct stream_instance* psi,HEVENT hEvent);

ULONG StrmRegStream(DDCMDREGISTER* packet);
ULONG StrmSetup(DDCMDSETUP* packet);
ULONG StrmRead(DDCMDREADWRITE* packet);
ULONG StrmWrite(DDCMDREADWRITE* packet);
ULONG StrmStatus(DDCMDSTATUS* packet);
ULONG StrmControl(DDCMDCONTROL* packet);
ULONG StrmDeregStream(DDCMDDEREGISTER* packet);

ULONG StrmStartStreaming(struct stream_instance* si);
ULONG StrmStopStreaming(struct stream_instance* si);

__inline__ ULONG getMS(void)
{
 struct timeval tv;
 do_gettimeofday(&tv);
 return tv.tv_usec;
}

#ifdef __cplusplus
}
#endif



#endif //STREAM_H_INCLUDED
