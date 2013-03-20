/*
 * strmidc.c
 * Autor:               Stefan Milcke
 * Erstellt am:         28.06.2003
 * Letzte Aenderung am: 30.07.2003
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
#include <linux/module.h>
#include <linux/kmod.h>

#include "stream.h"

stream_instance* cur_streams[BTTV_MAX]={0};
ULONG Min_Num_Buffs=2;

//------------------------------- StrmRegStream --------------------------------
ULONG StrmRegStream(DDCMDREGISTER* packet)
{
 ULONG rc=-1;
 struct stream_instance* psi=Strm_Add_SI((USHORT)(packet->ulSysFileNum)
                                         ,(USHORT)(packet->hStream));
 if(psi)
 {
  struct open_instance* poi=Strm_Find_OI(packet->ulSysFileNum);
  if(poi)
  {
   ULONG i;
   ULONG imgSize;
   psi->ulSysFileNum=packet->ulSysFileNum;
   psi->hStream=packet->hStream;
   psi->pSHDEntryPoint=packet->pSHDEntryPoint;
   // Calculate the size of 1 image + it's header for SI structure
   imgSize=((poi->CaptureRect.Dest_X_Width<<1)*poi->CaptureRect.Dest_Y_Height)
           +sizeof(VIDEO_FRAME_HDR);
   psi->ulImgSize=imgSize;
   psi->X_Left=poi->CaptureRect.Source_X_Left;
   psi->Y_Top=poi->CaptureRect.Source_Y_Top;
   psi->Y_Height=poi->CaptureRect.Source_Y_Height;
   psi->X_Width=poi->CaptureRect.Source_X_Width;
   psi->DX_Left=poi->CaptureRect.Dest_X_Left;
   psi->DY_Top=poi->CaptureRect.Dest_Y_Top;
   psi->DY_Height=poi->CaptureRect.Dest_Y_Height;
   psi->DX_Width=poi->CaptureRect.Dest_X_Width;
   psi->ScaleFactor=poi->CapISize.ScaleFactor;
   psi->ulFPS=poi->FPS.set_FPS;
   psi->ulFPSFlags=poi->FPS.ulFlags;
   packet->ulBufSize=imgSize;
   packet->ulNumBufs=Min_Num_Buffs;
   packet->ulAddressType=ADDRESS_TYPE_LINEAR;
   packet->ulBytesPerUnit=0;
   packet->mmtimePerUnit=0;
   rc=0;
  }
 }
 return rc;
}

//--------------------------------- StrmSetup ----------------------------------
ULONG StrmSetup(DDCMDSETUP* packet)
{
 ULONG rc=-1;
 struct stream_instance* psi=Strm_Find_SI(0,packet->hStream);
 if(psi)
 {
  struct open_instance* poi=Strm_Find_OI(psi->ulSysFileNum);
  if(poi)
  {
   SETUP_PARM* psu;
   DevVirtToLin(SELECTOROF(packet->pSetupParm)
                ,OFFSETOF(packet->pSetupParm)
                ,(LINEAR*)&psu);
   cur_streams[psi->devNo-1]=psi;     // Make this the current stream instance
   psi->ulSHTime=psu->ulStreamTime;
   psi->ulTime=psu->ulStreamTime;
   {
    ULONG ms=getMS();
    psi->ulSTime_Base=ms;
    psi->ulTime_Delta=ms;
   }
   psu->ulFlags=1;
   rc=0;
  }
 }
 return rc;
}

//---------------------------------- StrmRead ----------------------------------
ULONG StrmRead(DDCMDREADWRITE* packet)
{
 ULONG f;
 ULONG rc=-1;
 struct stream_instance* psi=Strm_Find_SI(0,packet->hStream);
 if(psi)
 {
  stream_instance_buffer* pbf=(stream_instance_buffer*)kmalloc(sizeof(stream_instance_buffer),0);
  pbf->bufLen=packet->ulBufferSize;
  pbf->pBuffer=packet->pBuffer;
  f=DevPushfCli();
  list_add_tail(&pbf->node,&psi->buff_list);
  DevPopf(f);
  rc=0;
 }
 return rc;
}

//--------------------------------- StrmWrite ----------------------------------
ULONG StrmWrite(DDCMDREADWRITE* packet)
{
 ULONG rc=-1;
 return rc;
}

//--------------------------------- StrmStatus ---------------------------------
ULONG StrmStatus(DDCMDSTATUS* packet)
{
 ULONG rc=-1;
 DebugInt3();
 return rc;
}

//------------------------------ StrmDeregStream -------------------------------
ULONG StrmDeregStream(DDCMDDEREGISTER* packet)
{
 ULONG rc=-1;
 struct stream_instance* p=Strm_Find_SI(0,packet->hStream);
 if(p)
 {
  StrmStopStreaming(p);
  Strm_Remove_SI(p->ulSysFileNum,p->hStream);
  rc=0;
 }
 return rc;
}

//-------------------------------- StrmIDCEntry --------------------------------
WORD32 StrmIDCEntry(int devNo,DDCMDCOMMON*  packet);
#pragma aux StrmIDCEntry "STREAM_IDC_ENTRY" parm [ecx] [ebx];
WORD32 StrmIDCEntry(int devNo,DDCMDCOMMON*  packet)
{
 WORD32 rc;
 if(packet)
 {
  switch(packet->ulFunction)
  {
   case DDCMD_REG_STREAM:
    rc=StrmRegStream((DDCMDREGISTER*)packet);
    break;
   case DDCMD_SETUP:
    rc=StrmSetup((DDCMDSETUP*)packet);
    break;
   case DDCMD_READ:
    rc=StrmRead((DDCMDREADWRITE*)packet);
    break;
   case DDCMD_WRITE:
    rc=StrmWrite((DDCMDREADWRITE*)packet);
    break;
   case DDCMD_STATUS:
    rc=StrmStatus((DDCMDSTATUS*)packet);
    break;
   case DDCMD_CONTROL:
    rc=StrmControl((DDCMDCONTROL*)packet);
    break;
   case DDCMD_DEREG_STREAM:
    rc=StrmDeregStream((DDCMDDEREGISTER*)packet);
    break;
   default:
    rc=ERROR_INVALID_FUNCTION;
    break;
  }
 }
 return rc;
}


#ifdef __cplusplus
};
#endif
