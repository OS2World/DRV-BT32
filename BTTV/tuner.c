/* $Id: tuner.c,v 1.16 2004/01/29 09:10:18 smilcke Exp $ */

/*
 * tuner.c
 * Autor:               Stefan Milcke
 * Erstellt am:         22.11.2001
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
#include <linux/videodev.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/init.h>
#include <linux/tvaudio.h>
#include <linux/process.h>
#include <media/audiochip.h>
#include <media/tuner.h>
#include <linux/i2c-compat.h>
#include "id.h"

#define strlcpy(dest,src,len) strncpy(dest,src,(len)-1)

// Addresses to scan
static unsigned short normal_i2c[] = {I2C_CLIENT_END};
static unsigned short normal_i2c_range[] = {0x60,0x6f,I2C_CLIENT_END};
I2C_CLIENT_INSMOD;

#define UNSET (-1)

// insmod options
static int debug =  0;
static int type  =  UNSET;
static int addr  =  0;
static char *pal =  "b";
static int tv_range[2]    = { 44, 958 };
static int radio_range[2] = { 65, 108 };

static unsigned long freq_stepping  = 16;

MODULE_PARM_LIST_HEAD(tuner_parms)
MODULE_PARM(type,"i")
MODULE_PARM(addr,"i")
MODULE_PARM_A(tv_range,"2i")
MODULE_PARM_A(radio_range,"2i")
MODULE_PARM(pal,"s")
MODULE_PARM_LIST_TAIL(tuner_parms)

// #define optimize_vco 1 (SM)
static int optimize_vco=1;

#ifdef TARGET_OS2
extern unsigned long hrmBttvDriver;
extern unsigned long hrmBttvAdapter;
static DEVICESTRUCT rmDeviceStruct=
{
 (unsigned char*)"TUNER",
 DS_FIXED_LOGICALNAME,
 DS_TYPE_AUDIO,
 NULL,
};
static struct lxrm_device rmDevice={0,0,0,&rmDeviceStruct};
#endif

static int this_adap;

struct tuner
{
 unsigned int type;     // chip type
 unsigned int freq;     // keep track of the current settings
 unsigned int std;

 unsigned int radio;
 unsigned int mode;     // current norm for multi-norm tuners
 unsigned int input;

 // only for MT2032
 unsigned int xogc;
 unsigned int radio_if2;

 void (*tv_freq)(struct i2c_client *c,unsigned int freq);
 void (*radio_freq)(struct i2c_client *c,unsigned int freq);
};

static int tuner_probe(struct i2c_adapter *adap);
static int tuner_detach(struct i2c_client *client);
static int tuner_command(struct i2c_client *client,unsigned int cmd,void *arg);
static struct i2c_driver driver=
{
 "i2c TV tuner driver",
 I2C_DRIVERID_TUNER,
 I2C_DF_NOTIFY,
 tuner_probe,
 tuner_detach,
 tuner_command,
};

static struct i2c_client client_template=
{
 "(tuner unset)",
 -1,
 I2C_CLIENT_ALLOW_USE,
 0,
 NULL,
 &driver,
 NULL,
 0,
};


/* tv standard selection for Temic 4046 FM5
   this value takes the low bits of control byte 2
   from datasheet Rev.01, Feb.00
     standard     BG      I       L       L2      D
     picture IF   38.9    38.9    38.9    33.95   38.9
     sound 1      33.4    32.9    32.4    40.45   32.4
     sound 2      33.16
     NICAM        33.05   32.348  33.05           33.05
 */
#define TEMIC_SET_PAL_I         0x05
#define TEMIC_SET_PAL_DK        0x09
#define TEMIC_SET_PAL_L         0x0a // SECAM ?
#define TEMIC_SET_PAL_L2        0x0b // change IF !
#define TEMIC_SET_PAL_BG        0x0c

/* tv tuner system standard selection for Philips FQ1216ME
   this value takes the low bits of control byte 2
   from datasheet "1999 Nov 16" (supersedes "1999 Mar 23")
     standard     BG DK I  L  L`
     picture carrier 38.90 38.90 38.90 38.90 33.95
     colour    34.47 34.47 34.47 34.47 38.38
     sound 1      33.40 32.40 32.90 32.40 40.45
     sound 2      33.16 -  -  -  -
     NICAM     33.05 33.05 32.35 33.05 39.80
 */
#define PHILIPS_SET_PAL_I  0x01 /* Bit 2 always zero !*/
#define PHILIPS_SET_PAL_BGDK  0x09
#define PHILIPS_SET_PAL_L2 0x0a
#define PHILIPS_SET_PAL_L  0x0b

/* system switching for Philips FI1216MF MK2
   from datasheet "1996 Jul 09",
 */
#define PHILIPS_MF_SET_BG  0x01 // Bit 2 must be zero, Bit 3 is system output
#define PHILIPS_MF_SET_PAL_L  0x03 // France
#define PHILIPS_MF_SET_PAL_L2 0x02 // L'


/* ---------------------------------------------------------------------- */

struct tunertype
{
   char *name;
   unsigned char Vendor;
   unsigned char Type;

   unsigned short thresh1;  //  band switch VHF_LO <=> VHF_HI
   unsigned short thresh2;  //  band switch VHF_HI <=> UHF
   unsigned char VHF_L;
   unsigned char VHF_H;
   unsigned char UHF;
   unsigned char config;
   unsigned short IFPCoff; /* 622.4=16*38.90 MHz PAL,
                              732=16*45.75 NTSCi
                              940=58.75 NTSC-Japan */
};

/*
 * The floats in the tuner struct are computed at compile time
 * by gcc and cast back to integers. Thus we don't violate the
 * "no float in kernel" rule.
 */
static struct tunertype tuners[] = {
 { "Temic PAL (4002 FH5)", TEMIC, PAL,
   16*140.25,16*463.25,0x02,0x04,0x01,0x8e,623},
 { "Philips PAL_I (FI1246 and compatibles)", Philips, PAL_I,
   16*140.25,16*463.25,0xa0,0x90,0x30,0x8e,623},
 { "Philips NTSC (FI1236,FM1236 and compatibles)", Philips, NTSC,
   16*157.25,16*451.25,0xA0,0x90,0x30,0x8e,732},
 { "Philips (SECAM+PAL_BG) (FI1216MF, FM1216MF, FR1216MF)", Philips, SECAM,
   16*168.25,16*447.25,0xA7,0x97,0x37,0x8e,623},

 { "NoTuner", NoTuner, NOTUNER,
   0,0,0x00,0x00,0x00,0x00,0x00},
 { "Philips PAL_BG (FI1216 and compatibles)", Philips, PAL,
   16*168.25,16*447.25,0xA0,0x90,0x30,0x8e,623},
 { "Temic NTSC (4032 FY5)", TEMIC, NTSC,
   16*157.25,16*463.25,0x02,0x04,0x01,0x8e,732},
 { "Temic PAL_I (4062 FY5)", TEMIC, PAL_I,
   16*170.00,16*450.00,0x02,0x04,0x01,0x8e,623},

 { "Temic NTSC (4036 FY5)", TEMIC, NTSC,
   16*157.25,16*463.25,0xa0,0x90,0x30,0x8e,732},
 { "Alps HSBH1", TEMIC, NTSC,
   16*137.25,16*385.25,0x01,0x02,0x08,0x8e,732},
 { "Alps TSBE1",TEMIC,PAL,
   16*137.25,16*385.25,0x01,0x02,0x08,0x8e,732},
 { "Alps TSBB5", Alps, PAL_I, /* tested (UK UHF) with Modulartech MM205 */
   16*133.25,16*351.25,0x01,0x02,0x08,0x8e,632},

 { "Alps TSBE5", Alps, PAL, /* untested - data sheet guess. Only IF differs. */
   16*133.25,16*351.25,0x01,0x02,0x08,0x8e,622},
 { "Alps TSBC5", Alps, PAL, /* untested - data sheet guess. Only IF differs. */
   16*133.25,16*351.25,0x01,0x02,0x08,0x8e,608},
 { "Temic PAL_BG (4006FH5)", TEMIC, PAL,
   16*170.00,16*450.00,0xa0,0x90,0x30,0x8e,623},
 { "Alps TSCH6",Alps,NTSC,
   16*137.25,16*385.25,0x14,0x12,0x11,0x8e,732},

 { "Temic PAL_DK (4016 FY5)",TEMIC,PAL,
   16*168.25,16*456.25,0xa0,0x90,0x30,0x8e,623},
 { "Philips NTSC_M (MK2)",Philips,NTSC,
   16*160.00,16*454.00,0xa0,0x90,0x30,0x8e,732},
 { "Temic PAL_I (4066 FY5)", TEMIC, PAL_I,
   16*169.00, 16*454.00, 0xa0,0x90,0x30,0x8e,623},
 { "Temic PAL* auto (4006 FN5)", TEMIC, PAL,
   16*169.00, 16*454.00, 0xa0,0x90,0x30,0x8e,623},

 { "Temic PAL_BG (4009 FR5) or PAL_I (4069 FR5)", TEMIC, PAL,
   16*141.00, 16*464.00, 0xa0,0x90,0x30,0x8e,623},
 { "Temic NTSC (4039 FR5)", TEMIC, NTSC,
   16*158.00, 16*453.00, 0xa0,0x90,0x30,0x8e,732},
 { "Temic PAL/SECAM multi (4046 FM5)", TEMIC, PAL,
   16*169.00, 16*454.00, 0xa0,0x90,0x30,0x8e,623},
 { "Philips PAL_DK (FI1256 and compatibles)", Philips, PAL,
   16*170.00,16*450.00,0xa0,0x90,0x30,0x8e,623},

 { "Philips PAL/SECAM multi (FQ1216ME)", Philips, PAL,
   16*170.00,16*450.00,0xa0,0x90,0x30,0x8e,623},
 { "LG PAL_I+FM (TAPC-I001D)", LGINNOTEK, PAL_I,
   16*170.00,16*450.00,0xa0,0x90,0x30,0x8e,623},
 { "LG PAL_I (TAPC-I701D)", LGINNOTEK, PAL_I,
   16*170.00,16*450.00,0xa0,0x90,0x30,0x8e,623},
 { "LG NTSC+FM (TPI8NSR01F)", LGINNOTEK, NTSC,
   16*210.00,16*497.00,0xa0,0x90,0x30,0x8e,732},

 { "LG PAL_BG+FM (TPI8PSB01D)", LGINNOTEK, PAL,
   16*170.00,16*450.00,0xa0,0x90,0x30,0x8e,623},
 { "LG PAL_BG (TPI8PSB11D)", LGINNOTEK, PAL,
   16*170.00,16*450.00,0xa0,0x90,0x30,0x8e,623},
 { "Temic PAL* auto + FM (4009 FN5)", TEMIC, PAL,
   16*141.00, 16*464.00, 0xa0,0x90,0x30,0x8e,623},
 { "SHARP NTSC_JP (2U5JF5540)", SHARP, NTSC, /* 940=16*58.75 NTSC@Japan */
   16*137.25,16*317.25,0x01,0x02,0x08,0x8e,732 }, // Corrected to NTSC=732 (was:940)

 { "Samsung PAL TCPM9091PD27", Samsung, PAL,  /* from sourceforge v3tv */
   16*169,16*464,0xA0,0x90,0x30,0x8e,623},
 { "MT20xx universal", Microtune,PAL|NTSC,
   0,0,0,0,0,0,0},
 { "Temic PAL_BG (4106 FH5)", TEMIC, PAL,
   16*141.00, 16*464.00, 0xa0,0x90,0x30,0x8e,623},
 { "Temic PAL_DK/SECAM_L (4012 FY5)", TEMIC, PAL,
   16*140.25, 16*463.25, 0x02,0x04,0x01,0x8e,623},

 { "Temic NTSC (4136 FY5)", TEMIC, NTSC,
   16*158.00, 16*453.00, 0xa0,0x90,0x30,0x8e,732},
 { "LG PAL (newer TAPC series)", LGINNOTEK, PAL,
   16*170.00, 16*450.00, 0x01,0x02,0x08,0x8e,623},
 { "Philips PAL/SECAM multi (FM1216ME MK3)", Philips, PAL,
   16*160.00,16*442.00,0x01,0x02,0x04,0x8e,623 },
 { "LG NTSC (newer TAPC series)", LGINNOTEK, NTSC,
   16*170.00, 16*450.00, 0x01,0x02,0x08,0x8e,732},

 { "HITACHI V7-J180AT", HITACHI, NTSC,
   16*170.00, 16*450.00, 0x01,0x02,0x00,0x8e,940 },
 { "Philips PAL_MK (FI1216 MK)", Philips, PAL,
   16*140.25,16*463.25,0x01,0xc2,0xcf,0x8e,623},
 { "Philips 1236D ATSC/NTSC daul in",Philips,ATSC,
   16*157.25,16*454.00,0xa0,0x90,0x30,0x8e,732},
};

#define TUNERS ARRAY_SIZE(tuners)

//------------------------------ tuner_getstatus -------------------------------
static int tuner_getstatus(struct i2c_client *c)
{
 unsigned char byte;
 struct tuner *t=(struct tuner*)i2c_get_clientdata(c);
 if(t->type==TUNER_MT2032)
  return 0;
 if(1!=i2c_master_recv(c,(char*)&byte,1))
  return 0;
 return byte;
}

#define TUNER_POR       0x80
#define TUNER_FL        0x40
#define TUNER_MODE      0x38
#define TUNER_AFC       0x07

#define TUNER_STEREO    0x10 // radio mode
#define TUNER_SIGNAL    0x07 // radio mode

//-------------------------------- tuner_signal --------------------------------
static int tuner_signal(struct i2c_client *c)
{
 return (tuner_getstatus(c) & TUNER_SIGNAL)<<13;
}

//-------------------------------- tuner_stereo --------------------------------
static int tuner_stereo(struct i2c_client *c)
{
 return (tuner_getstatus(c) & TUNER_STEREO);
}

//------------------------------- tuner_islocked -------------------------------
#if 0
static int tuner_islocked(struct i2c_client *c)
{
 return (tuner_getstatus(c) & TUNER_FL);
}

//------------------------------ tuner_afcstatus -------------------------------
static int tuner_afcstatus(struct i2c_client *c)
{
 return (tuner_getstatus(c) & TUNER_AFC) - 2;
}

//--------------------------------- tuner_mode ---------------------------------
static int tuner_mode(struct i2c_client *c)
{
 return (tuner_getstatus(c) & TUNER_MODE)>>3;
}
#endif

#define MT2032 0x04
#define MT2030 0x06
#define MT2040 0x07
#define MT2050 0x42

// IsSpurInBand()?
//------------------------------ mt2032_spurcheck ------------------------------
static int mt2032_spurcheck(int f1,int f2,int spectrum_from,int spectrum_to)
{
 int n1=1,n2,f;
 f1=f1/1000; //scale to kHz to avoid 32bit overflows
 f2=f2/1000;
 spectrum_from/=1000;
 spectrum_to/=1000;
 CPK(dprintk("spurcheck f1=%d f2=%d  from=%d to=%d\n"
             ,f1,f2,spectrum_from,spectrum_to));
 do
 {
  n2=-n1;
  f=n1*(f1-f2);
  do
  {
   n2--;
   f=f-f2;
   CPK(dprintk(" spurtest n1=%d n2=%d ftest=%d\n",n1,n2,f));
   if( (f>spectrum_from) && (f<spectrum_to))
    CPK(printk("mt2032 spurcheck triggered: %d\n",n1));
  }
  while ( (f>(f2-spectrum_to)) || (n2>-5));
  n1++;
 }
 while (n1<5);
 return 1;
}

//---------------------------- mt2032_compute_freq -----------------------------
static int mt2032_compute_freq(int rfin,int if1,int if2,int spectrum_from
                        ,int spectrum_to,unsigned char *buf,int *ret_sel,int xogc)
{
 unsigned int fref,lo1,lo1n,lo1a,s,sel,lo1freq
              ,desired_lo1,desired_lo2,lo2,lo2n,lo2a,lo2num,lo2freq;
 fref= 5250 *1000; //5.25MHz
 desired_lo1=rfin+if1;
 lo1=(2*(desired_lo1/1000)+(fref/1000)) / (2*fref/1000);
 lo1n=lo1/8;
 lo1a=lo1-(lo1n*8);
 s=rfin/1000/1000+1090;
 if(optimize_vco)
 {
  if(s>1890) sel=0;
  else if(s>1720) sel=1;
  else if(s>1530) sel=2;
  else if(s>1370) sel=3;
  else sel=4; // >1090
 }
 else
 {
  if(s>1790) sel=0; // <1958
  else if(s>1617) sel=1;
  else if(s>1449) sel=2;
  else if(s>1291) sel=3;
  else sel=4; // >1090
 }
 *ret_sel=sel;
 lo1freq=(lo1a+8*lo1n)*fref;
 CPK(dprintk("mt2032: rfin=%d lo1=%d lo1n=%d lo1a=%d sel=%d, lo1freq=%d\n",
      rfin,lo1,lo1n,lo1a,sel,lo1freq));
 desired_lo2=lo1freq-rfin-if2;
 lo2=(desired_lo2)/fref;
 lo2n=lo2/8;
 lo2a=lo2-(lo2n*8);
 //scale to fit in 32bit arith
 lo2num=((desired_lo2/1000)%(fref/1000))* 3780/(fref/1000);
 lo2freq=(lo2a+8*lo2n)*fref + lo2num*(fref/1000)/3780*1000;
 CPK(dprintk("mt2032: rfin=%d lo2=%d lo2n=%d lo2a=%d num=%d lo2freq=%d\n",
      rfin,lo2,lo2n,lo2a,lo2num,lo2freq));
 if(((int)lo1a)<0||lo1a>7||lo1n<17||lo1n>48
    || ((int)lo2a)<0 ||lo2a >7 ||lo2n<17 || lo2n>30)
 {
  CPK(printk("mt2032: frequency parameters out of range: %d %d %d %d\n"
             ,lo1a, lo1n, lo2a,lo2n));
  return(-1);
 }
 mt2032_spurcheck(lo1freq, desired_lo2,  spectrum_from, spectrum_to);
 // should recalculate lo1 (one step up/down)

 // set up MT2032 register map for transfer over i2c
 buf[0]=(char)(lo1n-1);
 buf[1]=(char)(lo1a | (sel<<4));
 buf[2]=0x86; // LOGC
 buf[3]=0x0f; //reserved
 buf[4]=0x1f;
 buf[5]=(char)((lo2n-1) | (lo2a<<5));
 if(rfin >400*1000*1000)
  buf[6]=0xe4;
 else
  buf[6]=0xf4; // set PKEN per rev 1.2
 buf[7]=8+xogc;
 buf[8]=0xc3; //reserved
 buf[9]=0x4e; //reserved
 buf[10]=0xec; //reserved
 buf[11]=(char)(lo2num&0xff);
 buf[12]=(char)((lo2num>>8) |0x80); // Lo2RST
 return 0;
}

//---------------------------- mt2032_check_lo_lock ----------------------------
static int mt2032_check_lo_lock(struct i2c_client *c)
{
 int tryl,lock=0;
 unsigned char buf[2];
 for(tryl=0;tryl<10;tryl++)
 {
  buf[0]=0x0e;
  i2c_master_send(c,(char*)buf,1);
  i2c_master_recv(c,(char*)buf,1);
  CPK(dprintk("mt2032 Reg.E=0x%02x\n",buf[0]));
  lock=buf[0] &0x06;
  if (lock==6)
   break;
  CPK(dprintk("mt2032: pll wait 1ms for lock (0x%2x)\n",buf[0]));
  udelay(1000);
 }
 return lock;
}

//---------------------------- mt2032_optimize_vco -----------------------------
static int mt2032_optimize_vco(struct i2c_client *c,int sel,int lock)
{
 unsigned char buf[2];
 int tad1;
 buf[0]=0x0f;
 i2c_master_send(c,(char*)buf,1);
 i2c_master_recv(c,(char*)buf,1);
 CPK(dprintk("mt2032 Reg.F=0x%02x\n",buf[0]));
 tad1=buf[0]&0x07;
 if(tad1 ==0) return lock;
 if(tad1 ==1) return lock;
 if(tad1==2)
 {
  if(sel==0)
   return lock;
  else sel--;
 }
 else
 {
  if(sel<4)
   sel++;
  else
   return lock;
 }
 CPK(dprintk("mt2032 optimize_vco: sel=%d\n",sel));
 buf[0]=0x0f;
 buf[1]=sel;
 i2c_master_send(c,(char*)buf,2);
 lock=mt2032_check_lo_lock(c);
 return lock;
}

//----------------------------- mt2032_set_if_freq -----------------------------
static void mt2032_set_if_freq(struct i2c_client *c,int rfin,int if1,int if2
                               ,int from,int to)
{
 unsigned char buf[21];
 int lint_try,ret,sel,lock=0;
 struct tuner *t = (struct tuner*)i2c_get_clientdata(c);
 CPK(dprintk("mt2032_set_if_freq rfin=%d if1=%d if2=%d from=%d to=%d\n"
             ,rfin,if1,if2,from,to));
 buf[0]=0;
 ret=i2c_master_send(c,(char*)buf,1);
 i2c_master_recv(c,(char*)buf,21);
 buf[0]=0;
 ret=mt2032_compute_freq(rfin,if1,if2,from,to,&buf[1],&sel,t->xogc);
 if (ret<0)
  return;
 // send only the relevant registers per Rev. 1.2
 buf[0]=0;
 ret=i2c_master_send(c,(char*)buf,4);
 buf[5]=5;
 ret=i2c_master_send(c,(char*)(buf+5),4);
 buf[11]=11;
 ret=i2c_master_send(c,(char*)(buf+11),3);
 if(ret!=3)
  CPK(printk("mt2032_set_if_freq failed with %d\n",ret));
 // wait for PLLs to lock (per manual), retry LINT if not.
 for(lint_try=0; lint_try<2; lint_try++)
 {
  lock=mt2032_check_lo_lock(c);
  if(optimize_vco)
   lock=mt2032_optimize_vco(c,sel,lock);
  if(lock==6)
   break;
  CPK(printk("mt2032: re-init PLLs by LINT\n"));
  buf[0]=7;
  buf[1]=(char)(0x80 +8+t->xogc); // set LINT to re-init PLLs
  i2c_master_send(c,(char*)buf,2);
  mdelay(10);
  buf[1]=(char)(8+t->xogc);
  i2c_master_send(c,(char*)buf,2);
 }
 if (lock!=6)
  CPK(printk("MT2032 Fatal Error: PLLs didn't lock.\n"));
 buf[0]=2;
 buf[1]=0x20; // LOGC for optimal phase noise
 ret=i2c_master_send(c,(char*)buf,2);
 if (ret!=2)
  CPK(printk("mt2032_set_if_freq2 failed with %d\n",ret));
}

//----------------------------- mt2032_set_tv_freq -----------------------------
static void mt2032_set_tv_freq(struct i2c_client *c,unsigned int freq)
{
 struct tuner* t=(struct tuner*)i2c_get_clientdata(c);
 int if2,from,to;
 // signal bandwidth and picture carrier
 if(t->mode==VIDEO_MODE_NTSC)
 {
  from=40750*1000;
  to=46750*1000;
  if2=45750*1000;
 }
 else
 {  // Pal
  from=32900*1000;
  to=39900*1000;
  if2=38900*1000;
 }
 mt2032_set_if_freq(c,freq*(1000*1000/freq_stepping)
                    ,1090*1000*1000, if2, from, to);
}

//--------------------------- mt2032_set_radio_freq ----------------------------
static void mt2032_set_radio_freq(struct i2c_client *c,unsigned int freq)
{
 struct tuner* t=(struct tuner*)i2c_get_clientdata(c);
 int if2=t->radio_if2;
 // per Manual for FM tuning: first if center freq. 1085 MHz
 mt2032_set_if_freq(c,freq*(1000*1000/freq_stepping), 1085*1000*1000,if2,if2,if2);
}
// Initialization as described in
// "MT203x Programming Procedures", Rev 1.2, Feb.2001
//-------------------------------- mt2032_init ---------------------------------
static int mt2032_init(struct i2c_client *c)
{
 unsigned char buf[21];
 int ret,xogc,xok=0;
 struct tuner *t = (struct tuner*)i2c_get_clientdata(c);
 // Initialize Registers per spec.
 buf[1]=2; // Index to register 2
 buf[2]=0xff;
 buf[3]=0x0f;
 buf[4]=0x1f;
 ret=i2c_master_send(c,(char*)(buf+1),4);

 buf[5]=6; // Index register 6
 buf[6]=0xe4;
 buf[7]=0x8f;
 buf[8]=0xc3;
 buf[9]=0x4e;
 buf[10]=0xec;
 ret=i2c_master_send(c,(char*)(buf+5),6);

 buf[12]=13;  // Index register 13
 buf[13]=0x32;
 ret=i2c_master_send(c,(char*)(buf+12),2);

 // Adjust XOGC (register 7), wait for XOK
 xogc=7;
 do
 {
  CPK(dprintk("mt2032: xogc = 0x%02x\n",xogc&0x07));
  mdelay(10);
  buf[0]=0x0e;
  i2c_master_send(c,(char*)(buf),1);
  i2c_master_recv(c,(char*)(buf),1);
  xok=buf[0]&0x01;
  CPK(dprintk("mt2032: xok = 0x%02x\n",xok));
  if (xok == 1) break;
  xogc--;
  CPK(dprintk("mt2032: xogc = 0x%02x\n",xogc&0x07));
  if (xogc == 3)
  {
   xogc=4; // min. 4 per spec
   break;
  }
  buf[0]=0x07;
  buf[1]=0x88 + xogc;
  ret=i2c_master_send(c,(char*)(buf),2);
  if (ret!=2)
   CPK(printk("mt2032_init failed with %d\n",ret));
 }while (xok != 1 );
 t->xogc=xogc;
 t->tv_freq    = mt2032_set_tv_freq;
 t->radio_freq = mt2032_set_radio_freq;
 return(1);
}

//----------------------------- mt2050_set_if_freq -----------------------------
static void mt2050_set_if_freq(struct i2c_client *c,unsigned int freq
                               ,unsigned int if2)
{
 unsigned int if1=1218*1000*1000;
 unsigned int f_lo1,f_lo2,lo1,lo2,f_lo1_modulo,f_lo2_modulo;
 unsigned int num1,num2,div1a,div1b,div2a,div2b;
 int ret;
 unsigned char buf[6];
 CPK(dprintk("mt2050_set_if_freq freq=%d\n",freq));
 f_lo1=freq+if1;
 f_lo1=(f_lo1/1000000)*1000000;
 f_lo2=f_lo1-freq-if2;
 f_lo2=(f_lo2/50000)*50000;
 lo1=f_lo1/4000000;
 lo2=f_lo2/4000000;
 f_lo1_modulo= f_lo1-(lo1*4000000);
 f_lo2_modulo= f_lo2-(lo2*4000000);
 num1=4*f_lo1_modulo/4000000;
 num2=4096*(f_lo2_modulo/1000)/4000;
 // todo spurchecks
 div1a=(lo1/12)-1;
 div1b=lo1-(div1a+1)*12;
 div2a=(lo2/8)-1;
 div2b=lo2-(div2a+1)*8;
 CPK(dprintk("lo1 lo2 = %d %d\n", lo1, lo2));
 CPK(dprintk("num1 num2 div1a div1b div2a div2b= %x %x %x %x %x %x\n"
             ,num1,num2,div1a,div1b,div2a,div2b));
 buf[0]=1;
 buf[1]=(char)(4*div1b + num1);
 if(freq<275*1000*1000) buf[1] = buf[1]|0x80;
 buf[2]=(char)div1a;
 buf[3]=(char)(32*div2b + num2/256);
 buf[4]=(char)(num2-(num2/256)*256);
 buf[5]=(char)div2a;
 if(num2!=0) buf[5]=buf[5]|0x40;
 if(debug)
 {
  int i;
  CPK(printk("bufs is: "));
  for(i=0;i<6;i++)
   CPK(printk("%x ",buf[i]));
  CPK(printk("\n"));
 }
 ret=i2c_master_send(c,(char*)buf,6);
 if (ret!=6)
  CPK(printk("mt2050_set_if_freq failed with %d\n",ret));
}

//----------------------------- mt2050_set_tv_freq -----------------------------
static void mt2050_set_tv_freq(struct i2c_client *c, unsigned int freq)
{
 struct tuner *t = (struct tuner*)i2c_get_clientdata(c);
 unsigned int if2;
 if (t->mode == VIDEO_MODE_NTSC)
 {
  if2=45750*1000;
 }
 else
 {
  // Pal
  if2=38900*1000;
 }
 mt2050_set_if_freq(c,freq*62500,if2);
}

//-------------------------------- mt2050_init ---------------------------------
static int mt2050_init(struct i2c_client *c)
{
 struct tuner *t = (struct tuner*)i2c_get_clientdata(c);
 unsigned char buf[1];
 int ret;

 buf[0]=6;
 buf[1]=0x10;
 ret=i2c_master_send(c,(char*)buf,2); //  power

 buf[0]=0x0f;
 buf[1]=0x0f;
 ret=i2c_master_send(c,(char*)buf,2); // m1lo

 buf[0]=0x0d;
 ret=i2c_master_send(c,(char*)buf,1);
 i2c_master_recv(c,(char*)buf,1);

 CPK(dprintk("mt2050: sro is %x\n",buf[0]));
 t->tv_freq = mt2050_set_tv_freq;
 return 0;
}

//------------------------------- microtune_init -------------------------------
static int microtune_init(struct i2c_client *c)
{
 struct tuner *t = (struct tuner*)i2c_get_clientdata(c);
 char *name;
 unsigned char buf[21];
 int company_code;
 buf[0] = 0;
 t->tv_freq = NULL;
 t->radio_freq = NULL;
 name = "unknown";
 i2c_master_send(c,(char*)buf,1);
 i2c_master_recv(c,(char*)buf,21);
 if(debug)
 {
  int i;
  CPK(printk(KERN_DEBUG "tuner: MT2032 hexdump:\n"));
  for(i=0;i<21;i++)
  {
   CPK(printk(" %02x",buf[i]));
   if(((i+1)%8)==0) CPK(printk(" "));
   if(((i+1)%16)==0) CPK(printk("\n "));
  }
  CPK(printk("\n "));
 }
 company_code = buf[0x11] << 8 | buf[0x12];
 CPK(printk("tuner: microtune: companycode=%04x part=%02x rev=%02x\n",
        company_code,buf[0x13],buf[0x14]));
 if (company_code != 0x4d54)
 {
  CPK(printk("tuner: microtune: unknown companycode\n"));
  return 0;
 }
/* Originally (SM)
 if (buf[0x13] < ARRAY_SIZE(microtune_part) &&
     NULL != microtune_part[buf[0x13]])
  name = microtune_part[buf[0x13]];
*/
 switch (buf[0x13])
 {
  case MT2030:
   name="MT2030";
   break;
  case MT2032:
   name="MT2032";
   mt2032_init(c);
   break;
  case MT2040:
   name="MT2040";
   break;
  case MT2050:
   name="MT2050";
   mt2050_init(c);
   break;
  default:
   CPK(printk("tuner: microtune %s found, not (yet?) supported, sorry :-/\n",
       name));
   return 0;
 }
 CPK(printk("tuner: microtune %s found, OK\n",name));
 return 0;
}

// Set tuner frequency
//---------------------------- default_set_tv_freq -----------------------------
static void default_set_tv_freq(struct i2c_client *c,unsigned int freq)
{
 u8 config;
 u16 di;
 struct tunertype *tun;
 struct tuner *t = (struct tuner*)i2c_get_clientdata(c);
 unsigned char buffer[4];
 int rc;

 tun=&tuners[t->type];
 if(freq < tun->thresh1)
  config = tun->VHF_L;
 else if(freq < tun->thresh2)
  config = tun->VHF_H;
 else
  config = tun->UHF;

 // tv norm specific stuff for multi-norm tuners
 switch (t->type)
 {
  case TUNER_PHILIPS_SECAM:
   // 0x01 -> ??? no change ??? */
   // 0x02 -> PAL BDGHI / SECAM L */
   // 0x04 -> ??? PAL others / SECAM others ???
   config &= ~0x02;
   if(t->mode == VIDEO_MODE_SECAM)
    config |= 0x02;
   break;
  case TUNER_TEMIC_4046FM5:
   config &= ~0x0f;
   switch(pal[0])
   {
    case 'i':
    case 'I':
     config |= TEMIC_SET_PAL_I;
     break;
    case 'd':
    case 'D':
     config |= TEMIC_SET_PAL_DK;
     break;
    case 'l':
    case 'L':
     config |= TEMIC_SET_PAL_L;
     break;
    case 'b':
    case 'B':
    case 'g':
    case 'G':
    default:
     config |= TEMIC_SET_PAL_BG;
     break;
   }
   break;
  case TUNER_PHILIPS_FQ1216ME:
   config &= ~0x0f;
   switch(pal[0])
   {
    case 'i':
    case 'I':
     config |= PHILIPS_SET_PAL_I;
     break;
    case 'l':
    case 'L':
     config |= PHILIPS_SET_PAL_L;
     break;
    case 'd':
    case 'D':
    case 'b':
    case 'B':
    case 'g':
    case 'G':
     config |= PHILIPS_SET_PAL_BGDK;
     break;
   }
   break;
 case TUNER_PHILIPS_ATSC:
  /* 0x00 -> ATSC antenna input 1 */
  /* 0x01 -> ATSC antenna input 2 */
  /* 0x02 -> NTSC antenna input 1 */
  /* 0x03 -> NTSC antenna input 2 */

  config &= ~0x03;
#ifdef VIDEO_MODE_ATSC
  if (VIDEO_MODE_ATSC != t->mode)
   config |= 2;
#endif
  /* FIXME: input */
  break;
 }
 /*
  * Philips FI1216MK2 remark from specification :
  * for channel selection involving band switching, and to ensure
  * smooth tuning to the desired channel without causing
  * unnecessary charge pump action, it is recommended to consider
  * the difference between wanted channel frequency and the
  * current channel frequency.  Unnecessary charge pump action
  * will result in very low tuning voltage which may drive the
  * oscillator to extreme conditions.
  *
  * Progfou: specification says to send config data before
  * frequency in case (wanted frequency < current frequency).
  */
 di=(u16)(freq + tun->IFPCoff);
 if(t->type == TUNER_PHILIPS_SECAM && freq < t->freq)
 {
  buffer[0] = tun->config;
  buffer[1] = config;
  buffer[2] = (di>>8) & 0x7f;
  buffer[3] = di      & 0xff;
 }
 else
 {
  buffer[0] = (di>>8) & 0x7f;
  buffer[1] = di      & 0xff;
  buffer[2] = tun->config;
  buffer[3] = config;
 }
 CPK(dprintk("tuner: tv 0x%02x 0x%02x 0x%02x 0x%02x\n",
      buffer[0],buffer[1],buffer[2],buffer[3]));
 if(4 != (rc = i2c_master_send(c,(char*)buffer,4)))
  CPK(printk("tuner: i2c i/o error: rc == %d (should be 4)\n",rc));

}

//--------------------------- default_set_radio_freq ---------------------------
static void default_set_radio_freq(struct i2c_client *c,unsigned int freq)
{
 struct tunertype *tun;
 struct tuner *t=(struct tuner*)i2c_get_clientdata(c);
 unsigned char buffer[4];
 u16 di;
 int rc;

 tun=&tuners[t->type];
 di=(u16)(freq+((freq_stepping*107)/10));
 buffer[0]=(di>>8)&0x7f;
 buffer[1]= di    &0xff;
 buffer[2]=tun->config;
 switch(t->type)
 {
  case TUNER_PHILIPS_FM1216ME_MK3:
   buffer[3]=0x19;
   break;
  default:
#ifdef TARGET_OS2
   buffer[3]=0xa8;
#else
   buffer[3]=0xa4;
#endif
   break;
 }
 CPK(dprintk("tuner: radio 0x%02x 0x%02x 0x%02x 0x%02x\n",
              buffer[0],buffer[1],buffer[2],buffer[3]));
 if(4!=(rc=i2c_master_send(c,(char*)buffer,4)))
  CPK(printk("tuner: i2c i/o error: rc==%d (should be 4)\n",rc));
#ifdef TARGET_OS2
 buffer[0]=tun->config;
 buffer[1]=0xa4;
 if(2!=(rc=i2c_master_send(c,(char*)buffer,2)))
  CPK(printk("tuner: i2c i/o error: rc==%d (should be 2)\n",rc));
#endif
}

// Set tuner frequency,  freq in Units of 62.5kHz = 1/freq_steppingMHz
//-------------------------------- set_tv_freq ---------------------------------
static void set_tv_freq(struct i2c_client *c, unsigned int freq)
{
 struct tuner *t=(struct tuner*)i2c_get_clientdata(c);

 if (t->type == UNSET)
 {
  CPK(printk("tuner: tuner type not set\n"));
  return;
 }
 if(NULL == t->tv_freq)
 {
  CPK(printk("tuner: Huh? tv_set is NULL?\n"));
  return;
 }
 if (freq < tv_range[0]*freq_stepping || freq > tv_range[1]*freq_stepping)
 {
  /* FIXME: better do that chip-specific, but
     right now we don't have that in the config
     struct and this way is still better than no
     check at all */
  CPK(printk("tuner: TV freq (%d.%02d) out of range (%d-%d)\n",
         freq/16,freq%freq_stepping*100/freq_stepping,tv_range[0],tv_range[1]));
  return;
 }
 t->tv_freq(c,freq);
}

//------------------------------- set_radio_freq -------------------------------
static void set_radio_freq(struct i2c_client *c, unsigned int freq)
{
 struct tuner *t=(struct tuner*)i2c_get_clientdata(c);

 if (t->type == UNSET)
 {
  CPK(printk("tuner: tuner type not set\n"));
  return;
 }
 if (NULL == t->radio_freq)
 {
  CPK(printk("tuner: no radio tuning for this one, sorry.\n"));
  return;
 }
 if (freq < radio_range[0]*freq_stepping || freq > radio_range[1]*freq_stepping)
 {
  CPK(printk("tuner: radio freq (%d.%02d) out of range (%d-%d)\n",
         freq/freq_stepping,freq%16*100/freq_stepping,
         radio_range[0],radio_range[1]));
  return;
 }
 t->radio_freq(c,freq);
}

//---------------------------------- set_type ----------------------------------
static void set_type(struct i2c_client *c, unsigned int type)
{
 struct tuner *t=(struct tuner*)i2c_get_clientdata(c);

 if (t->type != UNSET)
 {
  CPK(printk("tuner: type already set (%d)\n",t->type));
  return;
 }
 if (type >= TUNERS)
  return;
 t->type = type;
 CPK(printk("tuner: type set to %d (%s)\n", t->type,tuners[t->type].name));
 strlcpy(c->name, tuners[t->type].name, sizeof(c->name));
 switch (t->type)
 {
  case TUNER_MT2032:
   microtune_init(c);
   break;
  default:
   t->tv_freq    = default_set_tv_freq;
   t->radio_freq = default_set_radio_freq;
  break;
 }
}

//-------------------------------- tuner_attach --------------------------------
static int tuner_attach(struct i2c_adapter *adap,int addr
                        ,unsigned short flags,int kind)
{
 struct tuner *t;
 struct i2c_client *client;
 if(this_adap>0)
  return -1;
 this_adap++;
 client_template.adapter=adap;
 client_template.addr=addr;
 CPK(printk("tuner: chip found @ 0x%x\n",addr<<1));
 if(NULL==(client=(struct i2c_client*)kmalloc(sizeof(struct i2c_client),GFP_KERNEL)))
  return -ENOMEM;
 memcpy(client,&client_template,sizeof(struct i2c_client));
 t=(struct tuner*)kmalloc(sizeof(struct tuner),GFP_KERNEL);
 if(NULL==t)
 {
  kfree(client);
  return -ENOMEM;
 }
 memset(t,0,sizeof(struct tuner));
 i2c_set_clientdata(client,t);
 t->type=UNSET;
 t->radio_if2=10700*1000; // 10.7MHz - FM radio
#ifdef TARGET_OS2
 if(type<TUNERS)
  rmDeviceStruct.DevDescriptName=(PSZ)tuners[type].name;
#endif
 i2c_attach_client(client);
 if(type<TUNERS)
 {
		printk("tuner: type forced to %d (%s) [insmod]\n",
		       t->type,tuners[t->type].name);
  set_type(client,type);
 }
 if(t->type==TUNER_MT2032)
  mt2032_init(client);
 MOD_INC_USE_COUNT;
 return 0;
}

//-------------------------------- tuner_probe ---------------------------------
static int tuner_probe(struct i2c_adapter *adap)
{
 if (0 != addr)
 {
  normal_i2c_range[0] = addr;
  normal_i2c_range[1] = addr;
 }
 this_adap = 0;
 switch (adap->id)
 {
  case I2C_ALGO_BIT | I2C_HW_B_BT848:
  case I2C_ALGO_BIT | I2C_HW_B_RIVA:
  case I2C_ALGO_SAA7134:
  case I2C_ALGO_SAA7146:
   return i2c_probe(adap, &addr_data, tuner_attach);
 }
 return 0;
}

//-------------------------------- tuner_detach --------------------------------
static int tuner_detach(struct i2c_client *client)
{
 struct tuner *t=(struct tuner*)i2c_get_clientdata(client);
 i2c_detach_client(client);
 kfree(t);
 kfree(client);
 MOD_DEC_USE_COUNT;
 return 0;
}

//------------------------------- tuner_command --------------------------------
static int tuner_command(struct i2c_client *client,unsigned int cmd,void *arg)
{
 struct tuner *t=(struct tuner *)i2c_get_clientdata(client);
 unsigned int *iarg=(unsigned int *)arg;
 switch(cmd)
 {
  /* --- configuration --- */
  case TUNER_SET_TYPE:
   set_type(client,*iarg);
#ifdef TARGET_OS2
   rmDeviceStruct.DevDescriptName=(PSZ)tuners[t->type].name;
   if(0!=rmDevice.hDevice)
   {
    RM32DestroyDevice(rmDevice.hDriver,rmDevice.hDevice);
    rmDevice.hDevice=0;
   }
   RM32CreateDevice(rmDevice.hDriver,&(rmDevice.hDevice),&rmDeviceStruct
                    ,rmDevice.hAdapter,NULL);
#endif
   break;
  case AUDC_SET_RADIO:
   if(!t->radio)
   {
    set_tv_freq(client,400*freq_stepping);
    t->radio=1;
   }
   break;
  case AUDC_CONFIG_PINNACLE:
   switch(*iarg)
   {
    case 2:
     CPK(printk("tuner: pinnacle pal\n"));
     t->radio_if2=33300 * 1000;
     break;
    case 3:
     CPK(printk("tuner: pinnacle ntsc\n"));
     t->radio_if2=41300 * 1000;
     break;
   }
   break;


  // v4l ioctls
  // take car: bttv does userspace copying, we'll get a kernel pointer here...
  case VIDIOCSCHAN:
   {
    struct video_channel *vc=(struct video_channel*)arg;
    t->radio=0;
    t->mode=vc->norm;
    if(t->freq)
     set_tv_freq(client,t->freq);
    return 0;
   }
  case VIDIOCSFREQ:
   {
    unsigned long *v=(unsigned long*)arg;
    if(t->radio)
    {
     CPK(printk("tuner: radio freq set to %d.0%2d\n"
                ,(*iarg)/freq_stepping,(*iarg)%freq_stepping*100/freq_stepping));
     set_radio_freq(client,*v);
    }
    else
    {
     CPK(printk("tuner: tv freq set to %d.%02d\n"
                ,(*iarg)/freq_stepping,(*iarg)%freq_stepping*100/freq_stepping));
     set_tv_freq(client,*v);
    }
    t->freq=*v;
    return 0;
   }
  case VIDIOCGTUNER:
   {
    struct video_tuner *vt=(struct video_tuner*)arg;
    if(t->radio)
     vt->signal=tuner_signal(client);
    return 0;
   }
  case VIDIOCGAUDIO:
   {
    struct video_audio *va=(struct video_audio*)arg;
    if(t->radio)
     va->mode=(tuner_stereo(client) ? VIDEO_SOUND_STEREO : VIDEO_SOUND_MONO);
    return 0;
   }
#ifdef TARGET_OS2
  case VIDIOCSFREQSTEP:
   {
    int i;
    unsigned long IFPCoff;
    unsigned long *v=(unsigned long*)arg;
    unsigned long fs=*v;
    if(freq_stepping==fs)
     return 0;
    if(fs!=16 && fs!=20)
     return -EINVAL;
    for(i=0;i<TUNERS;i++)
    {
     if(0x8e==tuners[i].config)
      tuners[i].config=0x88;
     else if(0x88==tuners[i].config)
      tuners[i].config=0x8e;
     tuners[i].thresh1=(unsigned short)((tuners[i].thresh1*fs)/freq_stepping);
     tuners[i].thresh2=(unsigned short)((tuners[i].thresh2*fs)/freq_stepping);
     IFPCoff=tuners[i].IFPCoff;
     IFPCoff=(IFPCoff*fs)/freq_stepping;
     tuners[i].IFPCoff=(unsigned short)IFPCoff;
    }
    freq_stepping=*v;
    return 0;
   }
#endif
  default:
   // nothing
   break;
 }
 return 0;
}

//----------------------------- tuner_init_module ------------------------------
int tuner_init_module(void)
{
#ifdef TARGET_OS2
 driver.rm_device=&rmDevice;
 rmDevice.hDriver=hrmBttvDriver;
 rmDevice.hAdapter=hrmBttvAdapter;
#endif
 i2c_add_driver(&driver);
 return 0;
}

//---------------------------- tuner_cleanup_module ----------------------------
void tuner_cleanup_module(void)
{
 i2c_del_driver(&driver);
}

/******************************************************************************/
/* OS2 support                                                                */
/******************************************************************************/
static char NONE[]="None";
//-------------------------- OS2_tun_get_tuner_type ----------------------------
char *OS2_tun_get_tuner_type(int nr)
{
 if(nr>=0 && nr <TUNERS)
  return tuners[nr].name;
 return NONE;
}

#ifdef __cplusplus
}
#endif
