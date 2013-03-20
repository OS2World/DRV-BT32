/* $Id: tda9887.c,v 1.5 2004/01/29 09:10:17 smilcke Exp $ */

/*
 * tda9887.c
 * Autor:               Stefan Milcke
 * Erstellt am:         09.01.2003
 * Letzte Aenderung am: 29.01.2004
 *
*/
#ifdef __cplusplus
extern "C" {
#endif

#define INCL_NOPMAPI
#include <os2.h>
#include <lxrmcall.h>

#include <linux/types.h>
#include <linux/kernel.h>
//#include <linux/major.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/ioctl.h>
#include <linux/config.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/videodev.h>
#include <asm/semaphor.h>
#include <linux/init.h>
#include <ldefos2.h>
#include <linux/process.h>

#include "bttv.h"
#include <media/audiochip.h>
#include <linux/i2c-compat.h>

/* Chips:
   TDA9885 (PAL, NTSC)
   TDA9886 (PAL, SECAM, NTSC)
   TDA9887 (PAL, SECAM, NTSC, FM Radio)

   found on:
   - Pinnacle PCTV (Jul.2002 Version with MT2032, bttv)
      TDA9887 (world), TDA9885 (USA)
      Note: OP2 of tda988x must be set to 1, else MT2032 is disabled!
   - KNC One TV-Station RDS (saa7134)
*/
static unsigned short normal_i2c[] = {I2C_CLIENT_END};
static unsigned short normal_i2c_range[] = {0x86>>1,0x86>>1,I2C_CLIENT_END};
I2C_CLIENT_INSMOD;

/* insmod options */
static char *pal =  "b";
static char *secam =  "l";

MODULE_PARM_LIST_HEAD(tda9887_parms)
MODULE_PARM(pal,"s")
MODULE_PARM(secam,"s")
MODULE_PARM_LIST_TAIL(tda9887_parms)

struct tda9887
{
 struct i2c_client client;
 int radio,tvnorm;
 int pinnacle_id;
};

static int tda9887_probe(struct i2c_adapter *adap);
static int tda9887_detach(struct i2c_client *client);
static int tda9887_command(struct i2c_client *client, unsigned int cmd, void *arg);

static struct i2c_driver driver=
{
 "i2c tda9887 driver",
 -1, // FIXME
 I2C_DF_NOTIFY,
 tda9887_probe,
 tda9887_detach,
 tda9887_command,
};

static struct i2c_client client_template=
{
 "tda9887",
 -1,
 I2C_CLIENT_ALLOW_USE,
 0,
 NULL,
 &driver,
 NULL,
 0,
};

//
// TDA defines
//

//// first reg
#define cVideoTrapBypassOFF     0x00    // bit b0
#define cVideoTrapBypassON      0x01    // bit b0

#define cAutoMuteFmInactive     0x00    // bit b1
#define cAutoMuteFmActive       0x02    // bit b1

#define cIntercarrier           0x00    // bit b2
#define cQSS                    0x04    // bit b2

#define cPositiveAmTV           0x00    // bit b3:4
#define cFmRadio                0x08    // bit b3:4
#define cNegativeFmTV           0x10    // bit b3:4


#define cForcedMuteAudioON      0x20    // bit b5
#define cForcedMuteAudioOFF     0x00    // bit b5

#define cOutputPort1Active      0x00    // bit b6
#define cOutputPort1Inactive    0x40    // bit b6

#define cOutputPort2Active      0x00    // bit b7
#define cOutputPort2Inactive    0x80    // bit b7


//// second reg
#define cDeemphasisOFF          0x00    // bit c5
#define cDeemphasisON           0x20    // bit c5

#define cDeemphasis75           0x00    // bit c6
#define cDeemphasis50           0x40    // bit c6

#define cAudioGain0             0x00    // bit c7
#define cAudioGain6             0x80    // bit c7


//// third reg
#define cAudioIF_4_5             0x00    // bit e0:1
#define cAudioIF_5_5             0x01    // bit e0:1
#define cAudioIF_6_0             0x02    // bit e0:1
#define cAudioIF_6_5             0x03    // bit e0:1


#define cVideoIF_58_75           0x00    // bit e2:4
#define cVideoIF_45_75           0x04    // bit e2:4
#define cVideoIF_38_90           0x08    // bit e2:4
#define cVideoIF_38_00           0x0C    // bit e2:4
#define cVideoIF_33_90           0x10    // bit e2:4
#define cVideoIF_33_40           0x14    // bit e2:4
#define cRadioIF_45_75           0x18    // bit e2:4
#define cRadioIF_38_90           0x1C    // bit e2:4


#define cTunerGainNormal         0x00    // bit e5
#define cTunerGainLow            0x20    // bit e5

#define cGating_18               0x00    // bit e6
#define cGating_36               0x40    // bit e6

#define cAgcOutON                0x80    // bit e7
#define cAgcOutOFF               0x00    // bit e7

//-------------------------------- tda9887_miro --------------------------------
static int tda9887_miro(struct tda9887 *t)
{
   int rc;
   u8   bData[4]     = { 0 };
   u8   bVideoIF     = 0;
   u8   bAudioIF     = 0;
   u8   bDeEmphasis  = 0;
   u8   bDeEmphVal   = 0;
   u8   bModulation  = 0;
   u8   bCarrierMode = 0;
   u8   bOutPort1    = cOutputPort1Inactive;
#if 0
   u8   bOutPort2    = cOutputPort2Inactive & mbTADState; // store i2c tuner state
#else
   u8   bOutPort2    = cOutputPort2Inactive;
#endif
   u8   bVideoTrap   = cVideoTrapBypassOFF;
#if 1
   u8   bTopAdjust   = 0x0e /* -2dB */;
#else
   u8   bTopAdjust   = 0;
#endif

#if 0
   if (mParams.fVideoTrap)
      bVideoTrap   = cVideoTrapBypassON;
#endif

   if (t->radio) {
      bVideoTrap   = cVideoTrapBypassOFF;
      bCarrierMode = cQSS;
      bModulation  = cFmRadio;
      bOutPort1    = cOutputPort1Inactive;
      bDeEmphasis  = cDeemphasisON;
      if (3 == t->pinnacle_id) {
         /* ntsc */
         bDeEmphVal   = cDeemphasis75;
         bAudioIF     = cAudioIF_4_5;
         bVideoIF     = cRadioIF_45_75;
      } else {
         /* pal */
         bAudioIF     = cAudioIF_5_5;
         bVideoIF     = cRadioIF_38_90;
         bDeEmphVal   = cDeemphasis50;
      }

   } else if (t->tvnorm == VIDEO_MODE_PAL) {
      bDeEmphasis  = cDeemphasisON;
      bDeEmphVal   = cDeemphasis50;
      bModulation  = cNegativeFmTV;
      bOutPort1    = cOutputPort1Inactive;
      if (1 == t->pinnacle_id) {
         bCarrierMode = cIntercarrier;
      } else {
         // stereo boards
         bCarrierMode = cQSS;
      }
      switch (pal[0]) {
      case 'b':
      case 'g':
      case 'h':
         bVideoIF     = cVideoIF_38_90;
         bAudioIF     = cAudioIF_5_5;
         break;
      case 'd':
         bVideoIF     = cVideoIF_38_00;
         bAudioIF     = cAudioIF_6_5;
         break;
      case 'i':
         bVideoIF     = cVideoIF_38_90;
         bAudioIF     = cAudioIF_6_0;
         break;
      case 'm':
      case 'n':
         bVideoIF     = cVideoIF_45_75;
         bAudioIF     = cAudioIF_4_5;
         bDeEmphVal   = cDeemphasis75;
         if ((5 == t->pinnacle_id) || (6 == t->pinnacle_id)) {
            bCarrierMode = cIntercarrier;
         } else {
            bCarrierMode = cQSS;
         }
         break;
      }

   } else if (t->tvnorm == VIDEO_MODE_SECAM) {
      bAudioIF     = cAudioIF_6_5;
      bDeEmphasis  = cDeemphasisON;
      bDeEmphVal   = cDeemphasis50;
      bModulation  = cNegativeFmTV;
      bCarrierMode = cQSS;
      bOutPort1    = cOutputPort1Inactive;
      switch (secam[0]) {
      case 'd':
         bVideoIF     = cVideoIF_38_00;
         break;
      case 'k':
         bVideoIF     = cVideoIF_38_90;
         break;
      case 'l':
         bVideoIF     = cVideoIF_38_90;
         bDeEmphasis  = cDeemphasisOFF;
         bDeEmphVal   = cDeemphasis75;
         bModulation  = cPositiveAmTV;
         break;
      case 'L' /* L1 */:
         bVideoIF     = cVideoIF_33_90;
         bDeEmphasis  = cDeemphasisOFF;
         bDeEmphVal   = cDeemphasis75;
         bModulation  = cPositiveAmTV;
         break;
      }

   } else if (t->tvnorm == VIDEO_MODE_NTSC) {
                bVideoIF     = cVideoIF_45_75;
                bAudioIF     = cAudioIF_4_5;
                bDeEmphasis  = cDeemphasisON;
                bDeEmphVal   = cDeemphasis75;
                bModulation  = cNegativeFmTV;
                bOutPort1    = cOutputPort1Inactive;
                if ((5 == t->pinnacle_id) || (6 == t->pinnacle_id)) {
         bCarrierMode = cIntercarrier;
      } else {
         bCarrierMode = cQSS;
                }
   }

   bData[1] = bVideoTrap        |  // B0: video trap bypass
      cAutoMuteFmInactive  |  // B1: auto mute
      bCarrierMode         |  // B2: InterCarrier for PAL else QSS
      bModulation          |  // B3 - B4: positive AM TV for SECAM only
      cForcedMuteAudioOFF  |  // B5: forced Audio Mute (off)
      bOutPort1            |  // B6: Out Port 1
      bOutPort2;              // B7: Out Port 2
   bData[2] = bTopAdjust |   // C0 - C4: Top Adjust 0 == -16dB  31 == 15dB
      bDeEmphasis   |   // C5: De-emphasis on/off
      bDeEmphVal    |   // C6: De-emphasis 50/75 microsec
      cAudioGain0;      // C7: normal audio gain
   bData[3] = bAudioIF      |  // E0 - E1: Sound IF
      bVideoIF         |  // E2 - E4: Video IF
      cTunerGainNormal |  // E5: Tuner gain (normal)
      cGating_18       |  // E6: Gating (18%)
      cAgcOutOFF;         // E7: VAGC  (off)

   CPK(dprintk("tda9885/6/7: 0x%02x 0x%02x 0x%02x [pinnacle_id=%d]\n",
      bData[1],bData[2],bData[3],t->pinnacle_id));
   if (4 != (rc = i2c_master_send(&t->client,(char*)bData,4)))
   {
      CPK(printk("tda9885/6/7: i2c i/o error: rc == %d (should be 4)\n",rc));
   }
   return 0;
}

#if 0
/* just for reference: old knc-one saa7134 stuff */
static unsigned char buf_pal_bg[]    = { 0x00, 0x16, 0x70, 0x49 };
static unsigned char buf_pal_i[]     = { 0x00, 0x16, 0x70, 0x4a };
static unsigned char buf_pal_dk[]    = { 0x00, 0x16, 0x70, 0x4b };
static unsigned char buf_pal_l[]     = { 0x00, 0x06, 0x50, 0x4b };
static unsigned char buf_fm_stereo[] = { 0x00, 0x0e, 0x0d, 0x77 };
#endif
static unsigned char buf_pal_bg[]    = { 0x00, 0x96, 0x70, 0x49 };
static unsigned char buf_pal_i[]     = { 0x00, 0x96, 0x70, 0x4a };
static unsigned char buf_pal_dk[]    = { 0x00, 0x96, 0x70, 0x4b };
static unsigned char buf_pal_l[]     = { 0x00, 0x86, 0x50, 0x4b };
static unsigned char buf_fm_stereo[] = { 0x00, 0x8e, 0x0d, 0x77 };
static unsigned char buf_ntsc[]      = { 0x00, 0x96, 0x70, 0x44 };
static unsigned char buf_ntsc_jp[]   = { 0x00, 0x96, 0x70, 0x40 };

//----------------------------- tda9887_configure ------------------------------
static int tda9887_configure(struct tda9887 *t)
{
 unsigned char *buf = NULL;
 int rc;
 CPK(printk("tda9887_configure\n"));
 if (t->radio)
 {
  CPK(dprintk("tda9885/6/7: FM Radio mode\n"));
  buf = buf_fm_stereo;
 }
 else if (t->tvnorm == VIDEO_MODE_PAL)
 {
  CPK(dprintk("tda9885/6/7: PAL-%c mode\n",pal[0]));
  switch (pal[0])
  {
   case 'b':
   case 'g':
    buf = buf_pal_bg;
    break;
   case 'i':
    buf = buf_pal_i;
    break;
   case 'd':
   case 'k':
    buf = buf_pal_dk;
    break;
   case 'l':
    buf = buf_pal_l;
    break;
  }
 }
 else if (t->tvnorm == VIDEO_MODE_NTSC)
 {
  CPK(dprintk("tda9885/6/7: NTSC mode\n"));
  buf = buf_ntsc;
 }
 else if (t->tvnorm == VIDEO_MODE_SECAM)
 {
  CPK(dprintk("tda9885/6/7: SECAM mode\n"));
  buf = buf_pal_l;
 }
 else if (t->tvnorm == 6 /* BTTV hack */)
 {
  CPK(dprintk("tda9885/6/7: NTSC-Japan mode\n"));
  buf = buf_ntsc_jp;
 }
 if (NULL == buf)
 {
  CPK(printk("tda9885/6/7 unknown norm=%d\n",t->tvnorm));
  return 0;
 }
 CPK(dprintk("tda9885/6/7: 0x%02x 0x%02x 0x%02x\n",
      buf[1],buf[2],buf[3]));
 if (4 != (rc = i2c_master_send(&t->client,(char*)buf,4)))
  CPK(printk("tda9885/6/7: i2c i/o error: rc == %d (should be 4)\n",rc));
 return 0;
}

#ifdef TARGET_OS2
extern unsigned long hrmBttvDriver;
extern unsigned long hrmBttvAdapter;
static DEVICESTRUCT rmDeviceStruct=
{
 (unsigned char*)"TDA9887",
 DS_FIXED_LOGICALNAME,
 DS_TYPE_AUDIO,
 NULL,
};
static struct lxrm_device rmDevice={0,0,0,&rmDeviceStruct};
#endif

//------------------------------- tda9887_attach -------------------------------
static int tda9887_attach(struct i2c_adapter *adap, int addr,
           unsigned short flags, int kind)
{
 struct tda9887 *t;
 client_template.adapter = adap;
 client_template.addr    = addr;
 CPK(printk("tda9887: chip found @ 0x%x\n", addr<<1));
 if (NULL == (t = (struct tda9887*)kmalloc(sizeof(*t), GFP_KERNEL)))
  return -ENOMEM;
 memset(t,0,sizeof(*t));
 t->client = client_template;
 t->pinnacle_id = -1;
 t->tvnorm=VIDEO_MODE_PAL;
 i2c_set_clientdata(&t->client,t);
#ifdef TARGET_OS2
 rmDeviceStruct.DevDescriptName=(PSZ)"TDA9887";
#endif
 i2c_attach_client(&t->client);
 MOD_INC_USE_COUNT;
 return 0;
}

//------------------------------- tda9887_probe --------------------------------
static int tda9887_probe(struct i2c_adapter *adap)
{
 switch (adap->id)
 {
  case I2C_ALGO_BIT | I2C_HW_B_BT848:
  case I2C_ALGO_BIT | I2C_HW_B_RIVA:
//  case I2C_ALGO_SAA7134: // (SM)
   CPK(printk("tda9887: probing %s i2c adapter [id=0x%x]\n",
             adap->name,adap->id));
   return i2c_probe(adap, &addr_data, tda9887_attach);
  default:
   CPK(printk("tda9887: ignoring %s i2c adapter [id=0x%x]\n",
             adap->name,adap->id));
   break;
   /* nothing */
 }
 return 0;
}

//------------------------------- tda9887_detach -------------------------------
static int tda9887_detach(struct i2c_client *client)
{
 struct tda9887 *t = (struct tda9887*)i2c_get_clientdata(client);
 i2c_detach_client(client);
 kfree(t);
 MOD_DEC_USE_COUNT;
 return 0;
}

//------------------------------ tda9887_command -------------------------------
static int tda9887_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
 struct tda9887 *t = (struct tda9887*)client->data;
 switch (cmd)
 {
  /* --- configuration --- */
  case AUDC_SET_RADIO:
   t->radio = 1;
   if (-1 != t->pinnacle_id)
    tda9887_miro(t);
   else
    tda9887_configure(t);
   break;
  case AUDC_CONFIG_PINNACLE:
   {
    int *i = (int*)arg;
    t->pinnacle_id = *i;
    tda9887_miro(t);
    break;
   }
   /* --- v4l ioctls --- */
   /* take care: bttv does userspace copying, we'll get a
      kernel pointer here... */
  case VIDIOCSCHAN:
   {
    struct video_channel *vc = (struct video_channel*)arg;
    t->radio  = 0;
    t->tvnorm = vc->norm;
    if (-1 != t->pinnacle_id)
     tda9887_miro(t);
    else
     tda9887_configure(t);
    break;
   }
  default:
   /* nothing */
   break;
 }
 return 0;
}

//---------------------------- tda9887_init_module -----------------------------
int tda9887_init_module(void)
{
#ifdef TARGET_OS2
 driver.rm_device=&rmDevice;
 rmDevice.hDriver=hrmBttvDriver;
 rmDevice.hAdapter=hrmBttvAdapter;
#endif
 i2c_add_driver(&driver);
 return 0;
}

//--------------------------- tda9887_cleanup_module ---------------------------
void tda9887_cleanup_module(void)
{
 i2c_del_driver(&driver);
}

#ifdef __cplusplus
}
#endif
