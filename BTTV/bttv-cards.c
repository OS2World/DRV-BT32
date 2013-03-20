/* $Id: bttv-cards.c,v 1.16 2004/01/29 09:10:10 smilcke Exp $ */

/*
 * bttv-cards.c
 * Autor:               Stefan Milcke
 * Erstellt am:         02.11.2001
 * Letzte Aenderung am: 29.01.2004
 *
*/
#ifdef __cplusplus
extern "C" {
#endif

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
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/kmod.h>
#include <media/tuner.h>

#include "bttvp.h"
#include "bt832.h"

/* fwd decl */
static void boot_msp34xx(struct bttv *btv, int pin);
static void boot_bt832(struct bttv *btv);
static void hauppauge_eeprom(struct bttv *btv);
static void avermedia_eeprom(struct bttv *btv);
static void osprey_eeprom(struct bttv *btv);
static void modtec_eeprom(struct bttv *btv);
static void init_PXC200(struct bttv *btv);

static void winview_audio(struct bttv *btv, struct video_audio *v, int set);
static void lt9415_audio(struct bttv *btv, struct video_audio *v, int set);
static void avermedia_tvphone_audio(struct bttv *btv, struct video_audio *v,
        int set);
static void avermedia_tv_stereo_audio(struct bttv *btv, struct video_audio *v,
        int set);
static void terratv_audio(struct bttv *btv, struct video_audio *v, int set);
static void gvbctv3pci_audio(struct bttv *btv, struct video_audio *v, int set);
static void winfast2000_audio(struct bttv *btv, struct video_audio *v, int set);
static void pvbt878p9b_audio(struct bttv *btv, struct video_audio *v, int set);
static void fv2000s_audio(struct bttv *btv, struct video_audio *v, int set);
static void windvr_audio(struct bttv *btv, struct video_audio *v, int set);
static void adtvk503_audio(struct bttv *btv, struct video_audio *v, int set);
static void rv605_muxsel(struct bttv *btv, unsigned int input);
static void eagle_muxsel(struct bttv *btv, unsigned int input);
static void xguard_muxsel(struct bttv *btv, unsigned int input);
static void ivc120_muxsel(struct bttv *btv, unsigned int input);

static int terratec_active_radio_upgrade(struct bttv *btv);
static int tea5757_read(struct bttv *btv);
static int tea5757_write(struct bttv *btv, int value);
static void identify_by_eeprom(struct bttv *btv,
          unsigned char eeprom_data[256]);

/* config variables */
unsigned int triton1=0;
unsigned int vsfx=0;
unsigned int latency=UNSET;

unsigned int no_overlay=-1;
unsigned int card[BTTV_MAX] ={UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET};
unsigned int pll[BTTV_MAX]  ={UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET};
unsigned int tuner[BTTV_MAX]={UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET};
unsigned int svhs[BTTV_MAX] ={UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET,UNSET};
#ifdef MODULE
int autoload = 1;
#else
int autoload = 0;
#endif
int gpiomask = UNSET;
int audioall = UNSET;
int audiomux[5] = { UNSET, UNSET, UNSET, UNSET, UNSET };

/* ----------------------------------------------------------------------- */
/* list of card IDs for bt878+ cards                                       */

static struct CARD {
 unsigned id;
 int cardnr;
 char *name;
} cards[] __devinitdata = {
 { 0x13eb0070, BTTV_HAUPPAUGE878,  "Hauppauge WinTV" },
 { 0x39000070, BTTV_HAUPPAUGE878,  "Hauppauge WinTV-D" },
 { 0x45000070, BTTV_HAUPPAUGEPVR,  "Hauppauge WinTV/PVR" },
 { 0xff000070, BTTV_OSPREY1x0,     "Osprey-100" },
 { 0xff010070, BTTV_OSPREY2x0_SVID,"Osprey-200" },
 { 0xff020070, BTTV_OSPREY500,     "Osprey-500" },
 { 0xff030070, BTTV_OSPREY2000,    "Osprey-2000" },
 { 0xff040070, BTTV_OSPREY540,     "Osprey-540" },

 { 0x00011002, BTTV_ATI_TVWONDER,  "ATI TV Wonder" },
 { 0x00031002, BTTV_ATI_TVWONDERVE,"ATI TV Wonder/VE" },

 { 0x6606107d, BTTV_WINFAST2000,   "Leadtek WinFast TV 2000" },
 { 0x6607107d, BTTV_WINFAST2000,   "Leadtek WinFast VC 100" },
 { 0x263610b4, BTTV_STB2,          "STB TV PCI FM, Gateway P/N 6000704" },
 { 0x264510b4, BTTV_STB2,          "STB TV PCI FM, Gateway P/N 6000704" },
 { 0x402010fc, BTTV_GVBCTV3PCI,    "I-O Data Co. GV-BCTV3/PCI" },
 { 0x405010fc, BTTV_GVBCTV4PCI,    "I-O Data Co. GV-BCTV4/PCI" },
 { 0x407010fc, BTTV_GVBCTV5PCI,    "I-O Data Co. GV-BCTV5/PCI" },

 { 0x001211bd, BTTV_PINNACLE,      "Pinnacle PCTV" },
 { 0x001c11bd, BTTV_PINNACLESAT,   "Pinnacle PCTV Sat" },
 // some cards ship with byteswapped IDs ...
 { 0x1200bd11, BTTV_PINNACLE,      "Pinnacle PCTV [bswap]" },
 { 0xff00bd11, BTTV_PINNACLE,      "Pinnacle PCTV [bswap]" },

 { 0x3000121a, BTTV_VOODOOTV_FM,   "3Dfx VoodooTV FM/ VoodooTV 200" },
 { 0x3060121a, BTTV_STB2,   "3Dfx VoodooTV 100/ STB OEM" },

 { 0x3000144f, BTTV_MAGICTVIEW063, "(Askey Magic/others) TView99 CPH06x" },
 { 0x3002144f, BTTV_MAGICTVIEW061, "(Askey Magic/others) TView99 CPH05x" },
 { 0x3005144f, BTTV_MAGICTVIEW061, "(Askey Magic/others) TView99 CPH061/06L (T1/LC)" },
 { 0x5000144f, BTTV_MAGICTVIEW061, "Askey CPH050" },

 { 0x00011461, BTTV_AVPHONE98,     "AVerMedia TVPhone98" },
 { 0x00021461, BTTV_AVERMEDIA98,   "AVermedia TVCapture 98" },
 { 0x00031461, BTTV_AVPHONE98,     "AVerMedia TVPhone98" },
 { 0x00041461, BTTV_AVERMEDIA98,   "AVerMedia TVCapture 98" },
 { 0x03001461, BTTV_AVERMEDIA98,   "VDOMATE TV TUNER CARD" },

 { 0x300014ff, BTTV_MAGICTVIEW061, "TView 99 (CPH061)" },
 { 0x300214ff, BTTV_PHOEBE_TVMAS,  "Phoebe TV Master (CPH060)" },

 { 0x1117153b, BTTV_TERRATVALUE,   "Terratec TValue (Philips PAL B/G)" },
 { 0x1118153b, BTTV_TERRATVALUE,   "Terratec TValue (Temic PAL B/G)" },
 { 0x1119153b, BTTV_TERRATVALUE,   "Terratec TValue (Philips PAL I)" },
 { 0x111a153b, BTTV_TERRATVALUE,   "Terratec TValue (Temic PAL I)" },

 { 0x1123153b, BTTV_TERRATVRADIO,  "Terratec TV Radio+" },
 { 0x1127153b, BTTV_TERRATV,       "Terratec TV+ (V1.05)"    },
 // clashes with FlyVideo
 //{ 0x18521852, BTTV_TERRATV,     "Terratec TV+ (V1.10)"    },
 { 0x1134153b, BTTV_TERRATVALUE,   "Terratec TValue (LR102)" },
 { 0x1135153b, BTTV_TERRATVALUER,  "Terratec TValue Radio" }, // LR102
 { 0x5018153b, BTTV_TERRATVALUE,   "Terratec TValue" }, // ??

 { 0x400015b0, BTTV_ZOLTRIX_GENIE, "Zoltrix Genie TV" },
 { 0x400a15b0, BTTV_ZOLTRIX_GENIE, "Zoltrix Genie TV" },
 { 0x400d15b0, BTTV_ZOLTRIX_GENIE, "Zoltrix Genie TV / Radio" },
 { 0x401015b0, BTTV_ZOLTRIX_GENIE, "Zoltrix Genie TV / Radio" },
 { 0x401615b0, BTTV_ZOLTRIX_GENIE, "Zoltrix Genie TV / Radio" },

 { 0x1430aa00, BTTV_PV143,         "Provideo PV143A" },
 { 0x1431aa00, BTTV_PV143,         "Provideo PV143B" },
 { 0x1432aa00, BTTV_PV143,         "Provideo PV143C" },
 { 0x1433aa00, BTTV_PV143,         "Provideo PV143D" },

 { 0x1460aa00, BTTV_PV150,         "Provideo PV150A-1" },
 { 0x1461aa01, BTTV_PV150,         "Provideo PV150A-2" },
 { 0x1462aa02, BTTV_PV150,         "Provideo PV150A-3" },
 { 0x1463aa03, BTTV_PV150,         "Provideo PV150A-4" },

 { 0x1464aa04, BTTV_PV150,         "Provideo PV150B-1" },
 { 0x1465aa05, BTTV_PV150,         "Provideo PV150B-2" },
 { 0x1466aa06, BTTV_PV150,         "Provideo PV150B-3" },
 { 0x1467aa07, BTTV_PV150,         "Provideo PV150B-4" },

 { 0xa1550000, BTTV_IVC200,        "IVC-200" },
 { 0xa1550001, BTTV_IVC200,        "IVC-200" },
 { 0xa1550002, BTTV_IVC200,        "IVC-200" },
 { 0xa1550003, BTTV_IVC200,        "IVC-200" },
 { 0xa1550100, BTTV_IVC200,        "IVC-200G" },
 { 0xa1550101, BTTV_IVC200,        "IVC-200G" },
 { 0xa1550102, BTTV_IVC200,        "IVC-200G" },
 { 0xa1550103, BTTV_IVC200,        "IVC-200G" },

 { 0x41424344, BTTV_GRANDTEC,      "GrandTec Multi Capture" },
 { 0x01020304, BTTV_XGUARD,        "Grandtec Grand X-Guard" },

 { 0x010115cb, BTTV_GMV1,          "AG GMV1" },
 { 0x010114c7, BTTV_MODTEC_205,    "Modular Technology MM201/MM202/MM205/MM210/MM215 PCTV" },
 { 0x18501851, BTTV_CHRONOS_VS2,   "FlyVideo 98 (LR50)/ Chronos Video Shuttle II" },
 { 0x18511851, BTTV_FLYVIDEO98EZ,  "FlyVideo 98EZ (LR51)/ CyberMail AV" },
 { 0x18521852, BTTV_TYPHOON_TVIEW, "FlyVideo 98FM (LR50)/ Typhoon TView TV/FM Tuner" },
 { 0x10b42636, BTTV_HAUPPAUGE878,  "STB ???" },
 { 0x217d6606, BTTV_WINFAST2000,   "Leadtek WinFast TV 2000" },
 { 0x03116000, BTTV_SENSORAY311,   "Sensoray 311" },
 { 0x00790e11, BTTV_WINDVR,        "Canopus WinDVR PCI" },
 { 0xa0fca1a0, BTTV_ZOLTRIX,       "Face to Face Tvmax" },
 { 0x01010071, BTTV_NEBULA_DIGITV, "Nebula Electronics DigiTV" },

 // likely broken, vendor id doesn't match the other magic views ...
 //{ 0xa0fca04f, BTTV_MAGICTVIEW063, "Guillemot Maxi TV Video 3" },

 { 0, -1, 0 }
};

// array with description for bt848/bt878 tv/grabber cards
// (defined in bttv-cards.h created by CnvBttv.cmd)
#define NO_EXTERN_BTTV_TVCARDS
#include "bttv-cards.h"

unsigned char eeprom_data[256];

static char unknowntuner[]="Unknown";
//-------------------------------- bttv_idcard ---------------------------------
void __devinit bttv_idcard(struct bttv *btv)
{
 unsigned int gpiobits;
 int i,type;
 unsigned short tmp;
 btv->tuner_name=unknowntuner; // (SM)
 /* read PCI subsystem ID */
 pci_read_config_word(btv->dev, PCI_SUBSYSTEM_ID, &tmp);
 btv->cardid = tmp << 16;
 pci_read_config_word(btv->dev, PCI_SUBSYSTEM_VENDOR_ID, &tmp);
 btv->cardid |= tmp;
 if (0 != btv->cardid && 0xffffffff != btv->cardid)
 {
  /* look for the card */
  for (type = -1, i = 0; cards[i].id != 0; i++)
   if (cards[i].id  == btv->cardid)
    type = i;
  if (type != -1)
  {
   /* found it */
   CPK(printk(KERN_INFO "bttv%d: detected: %s [card=%d], "
          "PCI subsystem ID is %04x:%04x\n",
          btv->nr,cards[type].name,cards[type].cardnr,
          btv->cardid & 0xffff,
          (btv->cardid >> 16) & 0xffff));
   btv->type = cards[type].cardnr;
  }
  else
  {
   /* 404 */
   CPK(printk(KERN_INFO "bttv%d: subsystem: %04x:%04x (UNKNOWN)\n",
          btv->nr, btv->cardid & 0xffff,
          (btv->cardid >> 16) & 0xffff));
   CPK(printk(KERN_DEBUG "please mail id, board name and "
          "the correct card= insmod option to kraxel@bytesex.org\n"));
  }
 }
 /* let the user override the autodetected type */
 if (card[btv->nr] < bttv_num_tvcards)
  btv->type=card[btv->nr];

 /* print which card config we are using */
 sprintf(btv->video_dev.name,"BT%d%s(%.23s)",
         btv->id,
         (btv->id==848 && btv->revision==0x12) ? "A" : "",
         bttv_tvcards[btv->type].name);
 CPK(printk(KERN_INFO "bttv%d: using: %s [card=%d,%s]\n",btv->nr,
        btv->video_dev.name,btv->type,
        card[btv->nr] < bttv_num_tvcards
        ? "insmod option" : "autodetected"));
 /* overwrite gpio stuff ?? */
 if (UNSET == audioall && UNSET == audiomux[0])
  return;
 if (UNSET != audiomux[0])
 {
  gpiobits = 0;
  for (i = 0; i < 5; i++) {
   bttv_tvcards[btv->type].audiomux[i] = audiomux[i];
   gpiobits |= audiomux[i];
  }
 }
 else
 {
  gpiobits = audioall;
  for (i = 0; i < 5; i++)
  {
   bttv_tvcards[btv->type].audiomux[i] = audioall;
  }
 }
 bttv_tvcards[btv->type].gpiomask = (UNSET != gpiomask) ? gpiomask : gpiobits;
 CPK(printk(KERN_INFO "bttv%d: gpio config override: mask=0x%x, mux=",
        btv->nr,bttv_tvcards[btv->type].gpiomask));
 for (i = 0; i < 5; i++)
 {
  CPK(printk("%s0x%x", i ? "," : "", bttv_tvcards[btv->type].audiomux[i]));
 }
 CPK(printk("\n"));
}

/*
 * (most) board specific initialisations goes here
 */

/* Some Modular Technology cards have an eeprom, but no subsystem ID */
//----------------------------- identify_by_eeprom -----------------------------
void identify_by_eeprom(struct bttv *btv, unsigned char eeprom_data[256])
{
 int type = -1;
 if (0 == strncmp((char*)eeprom_data,"GET.MM20xPCTV",13))
  type = BTTV_MODTEC_205;
 else if (0 == strncmp((char*)eeprom_data+20,"Picolo",7))
  type = BTTV_EURESYS_PICOLO;
 else if (eeprom_data[0] == 0x84 && eeprom_data[2]== 0)
                type = BTTV_HAUPPAUGE; /* old bt848 */
 if (-1 != type)
 {
  btv->type = type;
  CPK(printk("bttv%d: detected by eeprom: %s [card=%d]\n",
         btv->nr, bttv_tvcards[btv->type].name, btv->type));
 }
}

//------------------------------- flyvideo_gpio --------------------------------
static void flyvideo_gpio(struct bttv *btv)
{
 int gpio,outbits,has_remote,has_radio,is_capture_only,is_lr90,has_tda9820_tda9821;
 int tuner=-1,ttype;
 outbits = btread(BT848_GPIO_OUT_EN);
 btwrite(0x00, BT848_GPIO_OUT_EN);
 udelay(8);  // without this we would see the 0x1800 mask
 gpio=btread(BT848_GPIO_DATA);
 btwrite(outbits, BT848_GPIO_OUT_EN);
 // all cards provide GPIO info, some have an additional eeprom
 // LR50: GPIO coding can be found lower right CP1 .. CP9
 //       CP9=GPIO23 .. CP1=GPIO15; when OPEN, the corresponding GPIO reads 1.
 //       GPIO14-12: n.c.
 // LR90: GP9=GPIO23 .. GP1=GPIO15 (right above the bt878)

 // lowest 3 bytes are remote control codes (no handshake needed)
        // xxxFFF: No remote control chip soldered
        // xxxF00(LR26/LR50), xxxFE0(LR90): Remote control chip (LVA001 or CF45) soldered
 // Note: Some bits are Audio_Mask !
 ttype=(gpio&0x0f0000)>>16;
 switch(ttype)
 {
  case 0x0: tuner=2; // NTSC, e.g. TPI8NSR11P
   break;
  case 0x2: tuner=39;// LG NTSC (newer TAPC series) TAPC-H701P
   break;
  case 0x4: tuner=5; // Philips PAL TPI8PSB02P, TPI8PSB12P, TPI8PSB12D or FI1216, FM1216
   break;
  case 0x6: tuner=37; // LG PAL (newer TAPC series) TAPC-G702P
   break;
  case 0xC: tuner=3; // Philips SECAM(+PAL) FQ1216ME or FI1216MF
   break;
  default:
   CPK(printk(KERN_INFO "bttv%d: FlyVideo_gpio: unknown tuner type.\n", btv->nr));
 }
 has_remote          =   gpio & 0x800000;
 has_radio     =   gpio & 0x400000;
 //   unknown                   0x200000;
 //   unknown2                  0x100000;
 is_capture_only     = !(gpio & 0x008000); //GPIO15
 has_tda9820_tda9821 = !(gpio & 0x004000);
 is_lr90             = !(gpio & 0x002000); // else LR26/LR50 (LR38/LR51 f. capture only)
 //          gpio & 0x001000 // output bit for audio routing
 if(is_capture_only)
  tuner=4; // No tuner present
 CPK(printk(KERN_INFO "bttv%d: FlyVideo Radio=%s RemoteControl=%s Tuner=%d gpio=0x%06x\n",
        btv->nr, has_radio? "yes":"no ", has_remote? "yes":"no ", tuner, gpio));
 CPK(printk(KERN_INFO "bttv%d: FlyVideo  LR90=%s tda9821/tda9820=%s capture_only=%s\n",
  btv->nr, is_lr90?"yes":"no ", has_tda9820_tda9821?"yes":"no ",
  is_capture_only?"yes":"no "));
 if(tuner!= -1) // only set if known tuner autodetected, else let insmod option through
  btv->tuner_type = tuner;
 btv->has_radio = has_radio;
 // LR90 Audio Routing is done by 2 hef4052, so Audio_Mask has 4 bits: 0x001c80
 // LR26/LR50 only has 1 hef4052, Audio_Mask 0x000c00
 // Audio options: from tuner, from tda9821/tda9821(mono,stereo,sap), from tda9874, ext., mute
 if(has_tda9820_tda9821) btv->audio_hook = lt9415_audio;
 //todo: if(has_tda9874) btv->audio_hook = fv2000s_audio;
}

int miro_tunermap[] = { 0,6,2,3,   4,5,6,0,  3,0,4,5,  5,2,16,1,
   14,2,17,1, 4,1,4,3,  1,2,16,1, 4,4,4,4 };
int miro_fmtuner[]  = { 0,0,0,0,   0,0,0,0,  0,0,0,0,  0,0,0,1,
   1,1,1,1,   1,1,1,0,  0,0,0,0,  0,1,0,0 };

//----------------------------- miro_pinnacle_gpio -----------------------------
static void miro_pinnacle_gpio(struct bttv *btv)
{
 int id,msp,gpio;
 char *info;
 btwrite(0,BT848_GPIO_OUT_EN);
 gpio = btread(BT848_GPIO_DATA);
 id   = ((gpio>>10) & 63) -1;
 msp  = bttv_I2CRead(btv, I2C_MSP3400, "MSP34xx");
 if (id < 32)
 {
  btv->tuner_type = miro_tunermap[id];
  if (0 == (gpio & 0x20))
  {
   btv->has_radio = 1;
   if (!miro_fmtuner[id])
   {
    btv->has_matchbox = 1;
    btv->mbox_we    = (1<<6);
    btv->mbox_most  = (1<<7);
    btv->mbox_clk   = (1<<8);
    btv->mbox_data  = (1<<9);
    btv->mbox_mask  = (1<<6)|(1<<7)|(1<<8)|(1<<9);
   }
  }
  else
  {
   btv->has_radio = 0;
  }
  if (-1 != msp)
  {
   if (btv->type == BTTV_MIRO)
    btv->type = BTTV_MIROPRO;
   if (btv->type == BTTV_PINNACLE)
    btv->type = BTTV_PINNACLEPRO;
  }
  CPK(printk(KERN_INFO
         "bttv%d: miro: id=%d tuner=%d radio=%s stereo=%s\n",
         btv->nr, id+1, btv->tuner_type,
         !btv->has_radio ? "no" :
         (btv->has_matchbox ? "matchbox" : "fmtuner"),
         (-1 == msp) ? "no" : "yes"));
 }
 else
 {
  /* new cards with microtune tuner */
  id = 63 - id;
  btv->has_radio = 0;
  switch (id)
  {
   case 1:
    info = "PAL / mono";
    break;
   case 2:
    info = "PAL+SECAM / stereo";
    btv->has_radio = 1;
    break;
   case 3:
    info = "NTSC / stereo";
    btv->has_radio = 1;
    break;
   case 4:
    info = "PAL+SECAM / mono";
    break;
   case 5:
    info = "NTSC / mono";
    break;
   case 6:
    info = "NTSC / stereo";
    break;
   default:
    info = "oops: unknown card";
    break;
  }
  if (-1 != msp)
   btv->type = BTTV_PINNACLEPRO;
  CPK(printk(KERN_INFO
         "bttv%d: pinnacle/mt: id=%d info=\"%s\" radio=%s\n",
         btv->nr, id, info, btv->has_radio ? "yes" : "no"));
  btv->tuner_type  = 33;
  btv->pinnacle_id = id;
 }
}

// GPIO21 L: Buffer aktiv, H: Buffer inaktiv
#define LM1882_SYNC_DRIVE  0x200000L

//------------------------------- init_ids_eagle -------------------------------
static void init_ids_eagle(struct bttv *btv)
{
 btwrite(0xFFFF37, BT848_GPIO_OUT_EN);
 btwrite(0x000000, BT848_GPIO_REG_INP);
 btwrite(0x200020, BT848_GPIO_DATA);
 /* flash strobe inverter ?! */
 btwrite(0x200024, BT848_GPIO_DATA);
 /* switch sync drive off */
 btor(LM1882_SYNC_DRIVE, BT848_GPIO_DATA);
 /* set BT848 muxel to 2 */
 btaor((2)<<5, ~(2<<5), BT848_IFORM);
}

/* Muxsel helper for the IDS Eagle.
 * the eagles does not use the standard muxsel-bits but
 * has its own multiplexer */
//-------------------------------- eagle_muxsel --------------------------------
static void eagle_muxsel(struct bttv *btv, unsigned int input)
{
 btaor((2)<<5, ~(3<<5), BT848_IFORM);
 btaor((bttv_tvcards[btv->type].muxsel[input&7]&3),
       ~3, BT848_GPIO_DATA);
#if 0
 /* svhs */
 /* wake chroma ADC */
 btand(~BT848_ADC_C_SLEEP, BT848_ADC);
 /* set to YC video */
 btor(BT848_CONTROL_COMP, BT848_E_CONTROL);
 btor(BT848_CONTROL_COMP, BT848_O_CONTROL);
#else
 /* composite */
 /* set chroma ADC to sleep */
 btor(BT848_ADC_C_SLEEP, BT848_ADC);
 /* set to composite video */
 btand(~BT848_CONTROL_COMP, BT848_E_CONTROL);
 btand(~BT848_CONTROL_COMP, BT848_O_CONTROL);
#endif
 /* switch sync drive off */
 btor(LM1882_SYNC_DRIVE, BT848_GPIO_DATA);
}

/* ----------------------------------------------------------------------- */

/* initialization part one -- before registering i2c bus */
//------------------------------ bttv_init_card1 -------------------------------
void __devinit bttv_init_card1(struct bttv *btv)
{
 switch (btv->type)
 {
  case BTTV_HAUPPAUGE:
  case BTTV_HAUPPAUGE878:
   boot_msp34xx(btv,5);
   break;
  case BTTV_VOODOOTV_FM:
   boot_msp34xx(btv,20);
   break;
  case BTTV_AVERMEDIA98:
   boot_msp34xx(btv,11);
   break;
  case BTTV_HAUPPAUGEPVR:
   pvr_boot(btv);
   break;
 }
}

/* initialization part two -- after registering i2c bus */
//------------------------------ bttv_init_card2 -------------------------------
void __devinit bttv_init_card2(struct bttv *btv)
{
 btv->tuner_type = -1;
 if (BTTV_UNKNOWN == btv->type)
 {
  bttv_readee(btv,eeprom_data,0xa0);
  identify_by_eeprom(btv,eeprom_data);
 }
 switch (btv->type)
 {
  case BTTV_MIRO:
  case BTTV_MIROPRO:
  case BTTV_PINNACLE:
  case BTTV_PINNACLEPRO:
   /* miro/pinnacle */
   miro_pinnacle_gpio(btv);
   break;
  case BTTV_FLYVIDEO_98:
  case BTTV_MAXI:
  case BTTV_LIFE_FLYKIT:
  case BTTV_FLYVIDEO:
  case BTTV_TYPHOON_TVIEW:
  case BTTV_CHRONOS_VS2:
  case BTTV_FLYVIDEO_98FM:
  case BTTV_FLYVIDEO2000:
  case BTTV_FLYVIDEO98EZ:
  case BTTV_CONFERENCETV:
  case BTTV_LIFETEC_9415:
   flyvideo_gpio(btv);
   break;
  case BTTV_HAUPPAUGE:
  case BTTV_HAUPPAUGE878:
  case BTTV_HAUPPAUGEPVR:
   /* pick up some config infos from the eeprom */
   bttv_readee(btv,eeprom_data,0xa0);
   hauppauge_eeprom(btv);
   break;
  case BTTV_AVERMEDIA98:
  case BTTV_AVPHONE98:
   bttv_readee(btv,eeprom_data,0xa0);
   avermedia_eeprom(btv);
   break;
  case BTTV_PXC200:
   init_PXC200(btv);
   break;
  case BTTV_VHX:
   btv->has_radio    = 1;
   btv->has_matchbox = 1;
   btv->mbox_we      = 0x20;
   btv->mbox_most    = 0;
   btv->mbox_clk     = 0x08;
   btv->mbox_data    = 0x10;
   btv->mbox_mask    = 0x38;
   break;
  case BTTV_VOBIS_BOOSTAR:
  case BTTV_TERRATV:
   terratec_active_radio_upgrade(btv);
   break;
  case BTTV_MAGICTVIEW061:
   if (btv->cardid == 0x3002144f)
   {
    btv->has_radio=1;
    CPK(printk("bttv%d: radio detected by subsystem id (CPH05x)\n",btv->nr));
   }
   break;
  case BTTV_STB2:
   if (btv->cardid == 0x3060121a)
   {
    /* Fix up entry for 3DFX VoodooTV 100,
       which is an OEM STB card variant. */
    btv->has_radio=0;
    btv->tuner_type=TUNER_TEMIC_NTSC;
   }
   break;
  case BTTV_OSPREY1x0:
  case BTTV_OSPREY1x0_848:
  case BTTV_OSPREY101_848:
  case BTTV_OSPREY1x1:
  case BTTV_OSPREY1x1_SVID:
  case BTTV_OSPREY2xx:
  case BTTV_OSPREY2x0_SVID:
  case BTTV_OSPREY2x0:
  case BTTV_OSPREY500:
  case BTTV_OSPREY540:
  case BTTV_OSPREY2000:
   bttv_readee(btv,eeprom_data,0xa0);
   osprey_eeprom(btv);
   break;
  case BTTV_IDS_EAGLE:
   init_ids_eagle(btv);
   break;
  case BTTV_MODTEC_205:
   bttv_readee(btv,eeprom_data,0xa0);
   modtec_eeprom(btv);
   break;
 }
 /* pll configuration */
 if (!(btv->id==848 && btv->revision==0x11))
 {
  /* defaults from card list */
  if (PLL_28 == bttv_tvcards[btv->type].pll)
  {
   btv->pll.pll_ifreq=28636363;
   btv->pll.pll_crystal=BT848_IFORM_XT0;
  }
  if (PLL_35 == bttv_tvcards[btv->type].pll)
  {
   btv->pll.pll_ifreq=35468950;
   btv->pll.pll_crystal=BT848_IFORM_XT1;
  }
  /* insmod options can override */
  switch (pll[btv->nr])
  {
   case 0: /* none */
    btv->pll.pll_crystal = 0;
    btv->pll.pll_ifreq   = 0;
    btv->pll.pll_ofreq   = 0;
    break;
   case 1: /* 28 MHz */
   case 28:
    btv->pll.pll_ifreq   = 28636363;
    btv->pll.pll_ofreq   = 0;
    btv->pll.pll_crystal = BT848_IFORM_XT0;
    break;
   case 2: /* 35 MHz */
   case 35:
    btv->pll.pll_ifreq   = 35468950;
    btv->pll.pll_ofreq   = 0;
    btv->pll.pll_crystal = BT848_IFORM_XT1;
    break;
  }
 }
 btv->pll.pll_current = -1;
 /* tuner configuration (from card list / autodetect / insmod option) */
 if (UNSET != bttv_tvcards[btv->type].tuner_type)
  if(UNSET == btv->tuner_type)
   btv->tuner_type = bttv_tvcards[btv->type].tuner_type;
 if (UNSET != tuner[btv->nr])
  btv->tuner_type = tuner[btv->nr];
 CPK(printk("bttv%d: using tuner=%d\n",btv->nr,btv->tuner_type));
 if (btv->pinnacle_id != UNSET)
  bttv_call_i2c_clients(btv,AUDC_CONFIG_PINNACLE,
                        &btv->pinnacle_id);
 if (btv->tuner_type != UNSET)
  bttv_call_i2c_clients(btv,TUNER_SET_TYPE,&btv->tuner_type);
 btv->svhs = bttv_tvcards[btv->type].svhs;
 if(svhs[btv->nr] != UNSET)
  btv->svhs=svhs[btv->nr];
 if (bttv_tvcards[btv->type].has_radio)
  btv->has_radio=1;
 if (bttv_tvcards[btv->type].audio_hook)
  btv->audio_hook=bttv_tvcards[btv->type].audio_hook;
 if (bttv_tvcards[btv->type].digital_mode == DIGITAL_MODE_CAMERA)
 {
  /* detect Bt832 chip for quartzsight digital camera */
  if ((bttv_I2CRead(btv, I2C_BT832_ALT1, "Bt832") >=0) ||
      (bttv_I2CRead(btv, I2C_BT832_ALT2, "Bt832") >=0))
   boot_bt832(btv);
 }
 /* try to detect audio/fader chips */
 if (!bttv_tvcards[btv->type].no_msp34xx &&
     bttv_I2CRead(btv, I2C_MSP3400, "MSP34xx") >=0)
 {
  if (autoload)
   request_module("msp3400");
 }
 if (bttv_tvcards[btv->type].msp34xx_alt &&
     bttv_I2CRead(btv, I2C_MSP3400_ALT, "MSP34xx (alternate address)") >=0)
 {
  if (autoload)
   request_module("msp3400");
 }
 if (!bttv_tvcards[btv->type].no_tda9875 &&
     bttv_I2CRead(btv, I2C_TDA9875, "TDA9875") >=0)
 {
  if (autoload)
   request_module("tda9875");
 }
 if (!bttv_tvcards[btv->type].no_tda7432 &&
     bttv_I2CRead(btv, I2C_TDA7432, "TDA7432") >=0)
 {
  if (autoload)
   request_module("tda7432");
 }
 if (bttv_tvcards[btv->type].needs_tvaudio)
 {
  if (autoload)
   request_module("tvaudio");
 }
 /* tuner modules */
 if (btv->pinnacle_id != UNSET)
 {
  if (autoload)
   request_module("tda9887");
 }
 if (btv->tuner_type != UNSET)
 {
  if (autoload)
   request_module("tuner");
 }
}

/* ----------------------------------------------------------------------- */
/* some hauppauge specific stuff                                           */

static struct HAUPPAUGE_TUNER
{
 int  id;
 char *name;
}
hauppauge_tuner[] __devinitdata =
{
 { TUNER_ABSENT,        "" },
 { TUNER_ABSENT,        "External" },
 { TUNER_ABSENT,        "Unspecified" },
 { TUNER_PHILIPS_PAL,   "Philips FI1216" },
 { TUNER_PHILIPS_SECAM, "Philips FI1216MF" },
 { TUNER_PHILIPS_NTSC,  "Philips FI1236" },
 { TUNER_PHILIPS_PAL_I, "Philips FI1246" },
 { TUNER_PHILIPS_PAL_DK,"Philips FI1256" },
 { TUNER_PHILIPS_PAL,   "Philips FI1216 MK2" },
 { TUNER_PHILIPS_SECAM, "Philips FI1216MF MK2" },
 { TUNER_PHILIPS_NTSC,  "Philips FI1236 MK2" },
 { TUNER_PHILIPS_PAL_I, "Philips FI1246 MK2" },
 { TUNER_PHILIPS_PAL_DK,"Philips FI1256 MK2" },
 { TUNER_TEMIC_NTSC,    "Temic 4032FY5" },
 { TUNER_TEMIC_PAL,     "Temic 4002FH5" },
 { TUNER_TEMIC_PAL_I,   "Temic 4062FY5" },
 { TUNER_PHILIPS_PAL,   "Philips FR1216 MK2" },
 { TUNER_PHILIPS_SECAM, "Philips FR1216MF MK2" },
 { TUNER_PHILIPS_NTSC,  "Philips FR1236 MK2" },
 { TUNER_PHILIPS_PAL_I, "Philips FR1246 MK2" },
 { TUNER_PHILIPS_PAL_DK,"Philips FR1256 MK2" },
 { TUNER_PHILIPS_PAL,   "Philips FM1216" },
 { TUNER_PHILIPS_SECAM, "Philips FM1216MF" },
 { TUNER_PHILIPS_NTSC,  "Philips FM1236" },
 { TUNER_PHILIPS_PAL_I, "Philips FM1246" },
 { TUNER_PHILIPS_PAL_DK,"Philips FM1256" },
 { TUNER_TEMIC_4036FY5_NTSC, "Temic 4036FY5" },
 { TUNER_ABSENT,        "Samsung TCPN9082D" },
 { TUNER_ABSENT,        "Samsung TCPM9092P" },
 { TUNER_TEMIC_4006FH5_PAL, "Temic 4006FH5" },
 { TUNER_ABSENT,        "Samsung TCPN9085D" },
 { TUNER_ABSENT,        "Samsung TCPB9085P" },
 { TUNER_ABSENT,        "Samsung TCPL9091P" },
 { TUNER_TEMIC_4039FR5_NTSC, "Temic 4039FR5" },
 { TUNER_PHILIPS_FQ1216ME,   "Philips FQ1216 ME" },
 { TUNER_TEMIC_4066FY5_PAL_I, "Temic 4066FY5" },
 { TUNER_ABSENT,        "Philips TD1536" },
 { TUNER_ABSENT,        "Philips TD1536D" },
 { TUNER_PHILIPS_NTSC,  "Philips FMR1236" }, /* mono radio */
 { TUNER_ABSENT,        "Philips FI1256MP" },
 { TUNER_ABSENT,        "Samsung TCPQ9091P" },
 { TUNER_TEMIC_4006FN5_MULTI_PAL, "Temic 4006FN5" },
 { TUNER_TEMIC_4009FR5_PAL, "Temic 4009FR5" },
 { TUNER_TEMIC_4046FM5,     "Temic 4046FM5" },
 { TUNER_TEMIC_4009FN5_MULTI_PAL_FM, "Temic 4009FN5" },
 { TUNER_ABSENT,        "Philips TD1536D_FH_44"},
 { TUNER_LG_NTSC_FM,    "LG TPI8NSR01F"},
 { TUNER_LG_PAL_FM,     "LG TPI8PSB01D"},
 { TUNER_LG_PAL,        "LG TPI8PSB11D"},
 { TUNER_LG_PAL_I_FM,   "LG TAPC-I001D"},
 { TUNER_LG_PAL_I,      "LG TAPC-I701D"}
};

//------------------------------- modtec_eeprom --------------------------------
static void modtec_eeprom(struct bttv *btv)
{
 if( strncmp((char*)&(eeprom_data[0x1e]),"Temic 4066 FY5",14) ==0)
 {
  btv->tuner_type=TUNER_TEMIC_4066FY5_PAL_I;
  CPK(printk("bttv Modtec: Tuner autodetected %s\n",
         &eeprom_data[0x1e]));
 }
 else
 {
  CPK(printk("bttv Modtec: Unknown TunerString:%s\n",
         &eeprom_data[0x1e]));
 }
}

//------------------------------ hauppauge_eeprom ------------------------------
static void __devinit hauppauge_eeprom(struct bttv *btv)
{
 unsigned int blk2,tuner,radio,model;
 if (eeprom_data[0] != 0x84 || eeprom_data[2] != 0)
  CPK(printk(KERN_WARNING "bttv%d: Hauppauge eeprom: invalid\n",
         btv->nr));
 /* Block 2 starts after len+3 bytes header */
 blk2 = eeprom_data[1] + 3;
 /* decode + use some config infos */
 model = eeprom_data[12] << 8 | eeprom_data[11];
 tuner = eeprom_data[9];
 radio = eeprom_data[blk2-1] & 0x01;
 if (tuner < ARRAY_SIZE(hauppauge_tuner))
 {
  btv->tuner_type = hauppauge_tuner[tuner].id;
  btv->tuner_name = hauppauge_tuner[tuner].name; // (SM)
 }
 if (radio)
  btv->has_radio = 1;
 if (bttv_verbose)
  CPK(printk(KERN_INFO "bttv%d: Hauppauge eeprom: model=%d, "
         "tuner=%s (%d), radio=%s\n",
         btv->nr, model, hauppauge_tuner[tuner].name,
         btv->tuner_type, radio ? "yes" : "no"));
}

//----------------------- terratec_active_radio_upgrade ------------------------
static int terratec_active_radio_upgrade(struct bttv *btv)
{
 int freq;
 btv->has_radio    = 1;
 btv->has_matchbox = 1;
 btv->mbox_we      = 0x10;
 btv->mbox_most    = 0x20;
 btv->mbox_clk     = 0x08;
 btv->mbox_data    = 0x04;
 btv->mbox_mask    = 0x3c;
 btv->mbox_iow     = 1 <<  8;
 btv->mbox_ior     = 1 <<  9;
 btv->mbox_csel    = 1 << 10;
 freq=88000/62.5;
 tea5757_write(btv, 5 * freq + 0x358); // write 0x1ed8
 if (0x1ed8 == tea5757_read(btv))
 {
  CPK(printk("bttv%d: Terratec Active Radio Upgrade found.\n",
         btv->nr));
  btv->has_radio    = 1;
  btv->has_matchbox = 1;
 }
 else
 {
  btv->has_radio    = 0;
  btv->has_matchbox = 0;
 }
 return 0;
}

// minimal bootstrap for the WinTV/PVR -- upload altera firmware.
// The hcwamc.rbf firmware file is on the Hauppauge driver CD. Have
// a look at Pvr/pvr45xxx.EXE (self-extracting zip archive, can be
// unpacked with unzip).
#define PVR_GPIO_DELAY     10

#define BTTV_ALT_DATA      0x000001
#define BTTV_ALT_DCLK      0x100000
#define BTTV_ALT_NCONFIG   0x800000

static char *firm_altera = "/usr/lib/video4linux/hcwamc.rbf";

//------------------------------ pvr_altera_load -------------------------------
static int __devinit pvr_altera_load(struct bttv *btv, u8 *micro, u32 microlen)
{
 u32 n;
 u8 bits;
 int i;
 btwrite(BTTV_ALT_DATA|BTTV_ALT_DCLK|BTTV_ALT_NCONFIG,
         BT848_GPIO_OUT_EN);
 btwrite(0,BT848_GPIO_DATA);
 udelay(PVR_GPIO_DELAY);
 btwrite(BTTV_ALT_NCONFIG,BT848_GPIO_DATA);
 udelay(PVR_GPIO_DELAY);
 for (n = 0; n < microlen; n++)
 {
  bits = micro[n];
  for ( i = 0 ; i < 8 ; i++ )
  {
   btand(~BTTV_ALT_DCLK,BT848_GPIO_DATA);
   if (bits & 0x01)
    btor(BTTV_ALT_DATA,BT848_GPIO_DATA);
   else
    btand(~BTTV_ALT_DATA,BT848_GPIO_DATA);
   btor(BTTV_ALT_DCLK,BT848_GPIO_DATA);
   bits >>= 1;
  }
 }
 btand(~BTTV_ALT_DCLK,BT848_GPIO_DATA);
 udelay(PVR_GPIO_DELAY);
 /* begin Altera init loop (Not necessary,but doesn't hurt) */
 for (i = 0 ; i < 30 ; i++)
 {
  btand(~BTTV_ALT_DCLK,BT848_GPIO_DATA);
  btor(BTTV_ALT_DCLK,BT848_GPIO_DATA);
 }
 btand(~BTTV_ALT_DCLK,BT848_GPIO_DATA);
 return 0;
}

//---------------------------------- pvr_boot ----------------------------------
int __devinit pvr_boot(struct bttv *btv)
{
 u32 microlen;
 u8 *micro;
 int result;
 // microlen = mod_firmware_load(firm_altera, (char**) &micro);
 microlen=0; // (SM)
 micro=0;    // (SM)
 if (!microlen)
 {
  CPK(printk(KERN_WARNING "bttv%d: altera firmware not found [%s]\n",
         btv->nr, firm_altera));
  return -1;
 }
 CPK(printk(KERN_INFO "bttv%d: uploading altera firmware [%s] ...\n",
        btv->nr, firm_altera));
 result = pvr_altera_load(btv, micro, microlen);
 CPK(printk(KERN_INFO "bttv%d: ... upload %s\n",
        btv->nr, (result < 0) ? "failed" : "ok"));
 vfree(micro);
 return result;
}

// some osprey specific stuff
//------------------------------- osprey_eeprom --------------------------------
static void __devinit osprey_eeprom(struct bttv *btv)
{
 int i = 0;
 unsigned char *ee = eeprom_data;
 unsigned long serial = 0;
 if (btv->type == 0)
 {
  /* this might be an antique... check for MMAC label in eeprom */
  if ((ee[0]=='M') && (ee[1]=='M') && (ee[2]=='A') && (ee[3]=='C'))
  {
   unsigned char checksum = 0;
   for (i =0; i<21; i++)
    checksum += ee[i];
   if (checksum != ee[21])
    return;
   btv->type = BTTV_OSPREY1x0_848;
   for (i = 12; i < 21; i++)
    serial *= 10, serial += ee[i] - '0';
  }
 }
 else
 {
  unsigned short type;
  int offset = 4*16;
  for(; offset < 8*16; offset += 16)
  {
   unsigned short checksum = 0;
   /* verify the checksum */
   for(i = 0; i<14; i++) checksum += ee[i+offset];
   checksum = ~checksum;  /* no idea why */
   if ((((checksum>>8)&0x0FF) == ee[offset+14]) &&
       ((checksum & 0x0FF) == ee[offset+15]))
   {
    break;
   }
  }
  if (offset >= 8*16)
   return;
  /* found a valid descriptor */
  type = (ee[offset+4]<<8) | (ee[offset+5]);
  switch(type)
  {
   /* 848 based */
   case 0x0004:
    btv->type = BTTV_OSPREY1x0_848;
    break;
   case 0x0005:
    btv->type = BTTV_OSPREY101_848;
    break;
   /* 878 based */
   case 0x0012:
   case 0x0013:
    btv->type = BTTV_OSPREY1x0;
    break;
   case 0x0014:
   case 0x0015:
    btv->type = BTTV_OSPREY1x1;
    break;
   case 0x0016:
   case 0x0017:
   case 0x0020:
    btv->type = BTTV_OSPREY1x1_SVID;
    break;
   case 0x0018:
   case 0x0019:
   case 0x001E:
   case 0x001F:
    btv->type = BTTV_OSPREY2xx;
    break;
   case 0x001A:
   case 0x001B:
    btv->type = BTTV_OSPREY2x0_SVID;
    break;
   case 0x0040:
    btv->type = BTTV_OSPREY500;
    break;
   case 0x0050:
   case 0x0056:
    btv->type = BTTV_OSPREY540;
    /* bttv_osprey_540_init(btv); */
    break;
   case 0x0060:
   case 0x0070:
    btv->type = BTTV_OSPREY2x0;
    //enable output on select control lines
    btwrite(0x000303, BT848_GPIO_OUT_EN);
    break;
   default:
    /* unknown...leave generic, but get serial # */
    break;
  }
  serial =  (ee[offset+6] << 24)
          | (ee[offset+7] << 16)
          | (ee[offset+8] <<  8)
          | (ee[offset+9]);
 }
 CPK(printk(KERN_INFO "bttv%d: osprey eeprom: card=%d name=%s serial=%ld\n",
     btv->nr, btv->type, bttv_tvcards[btv->type].name,serial));
}

/* ----------------------------------------------------------------------- */
/* AVermedia specific stuff, from  bktr_card.c                             */

int tuner_0_table[] = {
        TUNER_PHILIPS_NTSC,  TUNER_PHILIPS_PAL /* PAL-BG*/,
        TUNER_PHILIPS_PAL,   TUNER_PHILIPS_PAL /* PAL-I*/,
        TUNER_PHILIPS_PAL,   TUNER_PHILIPS_PAL,
        TUNER_PHILIPS_SECAM, TUNER_PHILIPS_SECAM,
        TUNER_PHILIPS_SECAM, TUNER_PHILIPS_PAL};
#if 0
int tuner_0_fm_table[] = {
        PHILIPS_FR1236_NTSC,  PHILIPS_FR1216_PAL,
        PHILIPS_FR1216_PAL,   PHILIPS_FR1216_PAL,
        PHILIPS_FR1216_PAL,   PHILIPS_FR1216_PAL,
        PHILIPS_FR1236_SECAM, PHILIPS_FR1236_SECAM,
        PHILIPS_FR1236_SECAM, PHILIPS_FR1216_PAL};
#endif

int tuner_1_table[] = {
        TUNER_TEMIC_NTSC,  TUNER_TEMIC_PAL,
        TUNER_TEMIC_PAL,   TUNER_TEMIC_PAL,
        TUNER_TEMIC_PAL,   TUNER_TEMIC_PAL,
        TUNER_TEMIC_4012FY5, TUNER_TEMIC_4012FY5, //TUNER_TEMIC_SECAM
        TUNER_TEMIC_4012FY5, TUNER_TEMIC_PAL};

//------------------------------ avermedia_eeprom ------------------------------
static void __devinit avermedia_eeprom(struct bttv *btv)
{
 int tuner_make,tuner_tv_fm,tuner_format,tuner=0;
 tuner_make      = (eeprom_data[0x41] & 0x7);
 tuner_tv_fm     = (eeprom_data[0x41] & 0x18) >> 3;
 tuner_format    = (eeprom_data[0x42] & 0xf0) >> 4;
 btv->has_remote = (eeprom_data[0x42] & 0x01);
 if (tuner_make == 0 || tuner_make == 2)
  if(tuner_format <=9)
   tuner = tuner_0_table[tuner_format];
 if (tuner_make == 1)
  if(tuner_format <=9)
   tuner = tuner_1_table[tuner_format];
 CPK(printk(KERN_INFO "bttv%d: Avermedia eeprom[0x%02x%02x]: tuner=",
  btv->nr,eeprom_data[0x41],eeprom_data[0x42]));
 if(tuner)
 {
  btv->tuner_type=tuner;
  CPK(printk("%d",tuner));
 }
 else
  CPK(printk("Unknown type"));
 CPK(printk(" radio:%s remote control:%s\n",
  tuner_tv_fm?"yes":"no",
  btv->has_remote?"yes":"no"));
}

// used on Voodoo TV/FM (Voodoo 200), S0 wired to 0x10000
//---------------------------- bttv_tda9880_setnorm ----------------------------
void bttv_tda9880_setnorm(struct bttv *btv, int norm)
{
 // fix up our card entry
 if(norm==VIDEO_MODE_NTSC)
 {
  bttv_tvcards[BTTV_VOODOOTV_FM].audiomux[0]=0x957fff;
  bttv_tvcards[BTTV_VOODOOTV_FM].audiomux[4]=0x957fff;
  CPK(dprintk("bttv_tda9880_setnorm to NTSC\n"));
 }
 else
 {
  bttv_tvcards[BTTV_VOODOOTV_FM].audiomux[0]=0x947fff;
  bttv_tvcards[BTTV_VOODOOTV_FM].audiomux[4]=0x947fff;
  CPK(dprintk("bttv_tda9880_setnorm to PAL\n"));
 }
 // set GPIO according
 btaor(bttv_tvcards[btv->type].audiomux[btv->audio],
       ~bttv_tvcards[btv->type].gpiomask, BT848_GPIO_DATA);
}

/*
 * reset/enable the MSP on some Hauppauge cards
 * Thanks to Kyösti Mälkki (kmalkki@cc.hut.fi)!
 *
 * Hauppauge:  pin  5
 * Voodoo:     pin 20
 */
//-------------------------------- boot_msp34xx --------------------------------
static void __devinit boot_msp34xx(struct bttv *btv, int pin)
{
 int mask = (1 << pin);
 btaor(mask, ~mask, BT848_GPIO_OUT_EN);
 btaor(0, ~mask, BT848_GPIO_DATA);
 udelay(2500);
 btaor(mask, ~mask, BT848_GPIO_DATA);
 if (bttv_gpio)
  bttv_gpio_tracking(btv,"msp34xx");
 if (bttv_verbose)
  CPK(printk(KERN_INFO "bttv%d: Hauppauge/Voodoo msp34xx: reset line "
         "init [%d]\n", btv->nr, pin));
}

//--------------------------------- boot_bt832 ---------------------------------
static void __devinit boot_bt832(struct bttv *btv)
{
 int outbits,databits,resetbit=0;
 switch (btv->type)
 {
  case BTTV_PXELVWPLTVPAK:
   resetbit = 0x400000;
   break;
  case BTTV_MODTEC_205:
   resetbit = 1<<9;
   break;
  default:
   BUG();
 }
 request_module("bt832");
 bttv_call_i2c_clients(btv, BT832_HEXDUMP, NULL);
 CPK(printk("bttv%d: Reset Bt832 [line=0x%x]\n",btv->nr,resetbit));
 btwrite(0, BT848_GPIO_DATA);
 outbits = btread(BT848_GPIO_OUT_EN);
 databits= btread(BT848_GPIO_DATA);
 btwrite(resetbit, BT848_GPIO_OUT_EN);
 udelay(5);
 btwrite(resetbit, BT848_GPIO_DATA);
 udelay(5);
 btwrite(0, BT848_GPIO_DATA);
 udelay(5);
 btwrite(outbits, BT848_GPIO_OUT_EN);
 btwrite(databits, BT848_GPIO_DATA);
 // bt832 on pixelview changes from i2c 0x8a to 0x88 after
 // being reset as above. So we must follow by this:
 bttv_call_i2c_clients(btv, BT832_REATTACH, NULL);
}

/* ----------------------------------------------------------------------- */
/*  Imagenation L-Model PXC200 Framegrabber */
/*  This is basically the same procedure as
 *  used by Alessandro Rubini in his pxc200
 *  driver, but using BTTV functions */
//-------------------------------- init_PXC200 ---------------------------------
static void __devinit init_PXC200(struct bttv *btv)
{
 static int vals[] __devinitdata = { 0x08, 0x09, 0x0a, 0x0b, 0x0d, 0x0d,
                                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                     0x00 };
 unsigned int i;
 int tmp;
 u32 val;
 /* Initialise GPIO-connevted stuff */
 btwrite(1<<13,BT848_GPIO_OUT_EN); /* Reset pin only */
 btwrite(0,BT848_GPIO_DATA);
 udelay(3);
 btwrite(1<<13,BT848_GPIO_DATA);
 /* GPIO inputs are pulled up, so no need to drive
  * reset pin any longer */
 btwrite(0,BT848_GPIO_OUT_EN);
 if (bttv_gpio)
  bttv_gpio_tracking(btv,"pxc200");
 /*  we could/should try and reset/control the AD pots? but
     right now  we simply  turned off the crushing.  Without
     this the AGC drifts drifts
     remember the EN is reverse logic -->
     setting BT848_ADC_AGC_EN disable the AGC
     tboult@eecs.lehigh.edu
 */
 btwrite(BT848_ADC_RESERVED|BT848_ADC_AGC_EN, BT848_ADC);
 /* Initialise MAX517 DAC */
 CPK(printk(KERN_INFO "Setting DAC reference voltage level ...\n"));
 bttv_I2CWrite(btv,0x5E,0,0x80,1);
 /* Initialise 12C508 PIC */
 /* The I2CWrite and I2CRead commmands are actually to the
  * same chips - but the R/W bit is included in the address
  * argument so the numbers are different */
 CPK(printk(KERN_INFO "Initialising 12C508 PIC chip ...\n"));
 /* First of all, enable the clock line. This is used in the PXC200-F */
 val = btread(BT848_GPIO_DMA_CTL);
 val |= BT848_GPIO_DMA_CTL_GPCLKMODE;
 btwrite(val, BT848_GPIO_DMA_CTL);
 /* Then, push to 0 the reset pin long enough to reset the *
  * device same as above for the reset line, but not the same
  * value sent to the GPIO-connected stuff
  * which one is the good one? */
 btwrite( (1<<2), BT848_GPIO_OUT_EN); /* only the reset pin */
 btwrite(0, BT848_GPIO_DATA);
 udelay(10);
 btwrite(1<<2, BT848_GPIO_DATA);
 for (i = 0; i < ARRAY_SIZE(vals); i++)
 {
  tmp=bttv_I2CWrite(btv,0x1E,0,vals[i],1);
  if (tmp != -1)
  {
   CPK(printk(KERN_INFO
          "I2C Write(%2.2x) = %i\nI2C Read () = %2.2x\n\n",
          vals[i],tmp,bttv_I2CRead(btv,0x1F,NULL)));
  }
 }
 CPK(printk(KERN_INFO "PXC200 Initialised.\n"));
}

/* ----------------------------------------------------------------------- */
/* Miro Pro radio stuff -- the tea5757 is connected to some GPIO ports     */
/*
 * Copyright (c) 1999 Csaba Halasz <qgehali@uni-miskolc.hu>
 * This code is placed under the terms of the GNU General Public License
 *
 * Brutally hacked by Dan Sheridan <dan.sheridan@contact.org.uk> djs52 8/3/00
 */
//---------------------------------- bus_low -----------------------------------
void bus_low(struct bttv *btv, int bit)
{
 if (btv->mbox_ior)
 {
  btor(btv->mbox_ior | btv->mbox_iow | btv->mbox_csel,
       BT848_GPIO_DATA);
  udelay(5);
 }
 btand(~(bit), BT848_GPIO_DATA);
 udelay(5);
 if (btv->mbox_ior)
 {
  btand(~(btv->mbox_iow | btv->mbox_csel),
        BT848_GPIO_DATA);
  udelay(5);
 }
}

//---------------------------------- bus_high ----------------------------------
void bus_high(struct bttv *btv, int bit)
{
 if (btv->mbox_ior)
 {
  btor(btv->mbox_ior | btv->mbox_iow | btv->mbox_csel,
       BT848_GPIO_DATA);
  udelay(5);
 }
 btor((bit), BT848_GPIO_DATA);
 udelay(5);
 if (btv->mbox_ior)
 {
  btand(~(btv->mbox_iow | btv->mbox_csel),
        BT848_GPIO_DATA);
  udelay(5);
 }
}

//----------------------------------- bus_in -----------------------------------
int bus_in(struct bttv *btv, int bit)
{
 if (btv->mbox_ior)
 {
  btor(btv->mbox_ior | btv->mbox_iow | btv->mbox_csel,
       BT848_GPIO_DATA);
  udelay(5);
  btand(~(btv->mbox_ior | btv->mbox_csel),
        BT848_GPIO_DATA);
  udelay(5);
 }
 return btread(BT848_GPIO_DATA) & (bit);
}

/* TEA5757 register bits */
#define TEA_FREQ  0:14
#define TEA_BUFFER  15:15

#define TEA_SIGNAL_STRENGTH 16:17

#define TEA_PORT1  18:18
#define TEA_PORT0  19:19

#define TEA_BAND  20:21
#define TEA_BAND_FM  0
#define TEA_BAND_MW  1
#define TEA_BAND_LW  2
#define TEA_BAND_SW  3

#define TEA_MONO  22:22
#define TEA_ALLOW_STEREO 0
#define TEA_FORCE_MONO  1

#define TEA_SEARCH_DIRECTION 23:23
#define TEA_SEARCH_DOWN  0
#define TEA_SEARCH_UP  1

#define TEA_STATUS  24:24
#define TEA_STATUS_TUNED 0
#define TEA_STATUS_SEARCHING 1

//-------------------------------- tea5757_read --------------------------------
static int tea5757_read(struct bttv *btv)
{
 unsigned long timeout;
 int value = 0;
 int i;
 /* better safe than sorry */
 btaor((btv->mbox_clk | btv->mbox_we),
       ~btv->mbox_mask, BT848_GPIO_OUT_EN);
 if (btv->mbox_ior)
 {
  btor(btv->mbox_ior | btv->mbox_iow | btv->mbox_csel,
       BT848_GPIO_DATA);
  udelay(5);
 }
 if (bttv_gpio)
  bttv_gpio_tracking(btv,"tea5757 read");
 bus_low(btv,btv->mbox_we);
 bus_low(btv,btv->mbox_clk);
 udelay(10);
 timeout= jiffies + HZ;
 // wait for DATA line to go low; error if it doesn't
 while (bus_in(btv,btv->mbox_data) && time_before(jiffies, timeout))
  schedule();
 if (bus_in(btv,btv->mbox_data))
 {
  CPK(printk(KERN_WARNING "bttv%d: tea5757: read timeout\n",btv->nr));
  return -1;
 }
 CPK(dprintk("bttv%d: tea5757:",btv->nr));
 for(i = 0; i < 24; i++)
 {
  udelay(5);
  bus_high(btv,btv->mbox_clk);
  udelay(5);
  CPK(dprintk("%c",(bus_in(btv,btv->mbox_most) == 0)?'T':'-'));
  bus_low(btv,btv->mbox_clk);
  value <<= 1;
  value |= (bus_in(btv,btv->mbox_data) == 0)?0:1;  /* MSB first */
  CPK(dprintk("%c", (bus_in(btv,btv->mbox_most) == 0)?'S':'M'));
 }
 CPK(dprintk("\nbttv%d: tea5757: read 0x%X\n", btv->nr, value));
 return value;
}

//------------------------------- tea5757_write --------------------------------
static int tea5757_write(struct bttv *btv,int value)
{
 int i;
 int reg = value;
 btaor(btv->mbox_clk | btv->mbox_we | btv->mbox_data,
       ~btv->mbox_mask, BT848_GPIO_OUT_EN);
 if (btv->mbox_ior)
 {
  btor(btv->mbox_ior | btv->mbox_iow | btv->mbox_csel,
       BT848_GPIO_DATA);
  udelay(5);
 }
 if (bttv_gpio)
  bttv_gpio_tracking(btv,"tea5757 write");

 CPK(dprintk("bttv%d: tea5757: write 0x%X\n", btv->nr, value));
 bus_low(btv,btv->mbox_clk);
 bus_high(btv,btv->mbox_we);
 for(i = 0; i < 25; i++)
 {
  if (reg & 0x1000000)
   bus_high(btv,btv->mbox_data);
  else
   bus_low(btv,btv->mbox_data);
  reg <<= 1;
  bus_high(btv,btv->mbox_clk);
  udelay(10);
  bus_low(btv,btv->mbox_clk);
  udelay(10);
 }
 bus_low(btv,btv->mbox_we);  /* unmute !!! */
 return 0;
}

//------------------------------ tea5757_set_freq ------------------------------
void tea5757_set_freq(struct bttv *btv, unsigned short freq)
{
 CPK(dprintk("tea5757_set_freq %d\n",freq));
 tea5757_write(btv, 5 * freq + 0x358); /* add 10.7MHz (see docs) */
#if 0
 /* breaks Miro PCTV */
 value = tea5757_read(btv);
 CPK(dprintk("bttv%d: tea5757 readback=0x%x\n",btv->nr,value));
#endif
}

/* ----------------------------------------------------------------------- */
/* winview                                                                 */
//------------------------------- winview_audio --------------------------------
void winview_audio(struct bttv *btv,struct video_audio *v,int set)
{
 /* PT2254A programming Jon Tombs, jon@gte.esi.us.es */
 int bits_out, loops, vol, data;
 if (!set)
 {
  /* Fixed by Leandro Lucarella <luca@linuxmendoza.org.ar (07/31/01) */
  v->flags |= VIDEO_AUDIO_VOLUME;
  return;
 }
 /* 32 levels logarithmic */
 vol = 32 - ((v->volume>>11));
 /* units */
 bits_out = (PT2254_DBS_IN_2>>(vol%5));
 /* tens */
 bits_out |= (PT2254_DBS_IN_10>>(vol/5));
 bits_out |= PT2254_L_CHANNEL | PT2254_R_CHANNEL;
 data = btread(BT848_GPIO_DATA);
 data &= ~(WINVIEW_PT2254_CLK| WINVIEW_PT2254_DATA|
         WINVIEW_PT2254_STROBE);
 for (loops = 17; loops >= 0 ; loops--)
 {
  if (bits_out & (1<<loops))
   data |=  WINVIEW_PT2254_DATA;
  else
   data &= ~WINVIEW_PT2254_DATA;
  btwrite(data, BT848_GPIO_DATA);
  udelay(5);
  data |= WINVIEW_PT2254_CLK;
  btwrite(data, BT848_GPIO_DATA);
  udelay(5);
  data &= ~WINVIEW_PT2254_CLK;
  btwrite(data, BT848_GPIO_DATA);
 }
 data |=  WINVIEW_PT2254_STROBE;
 data &= ~WINVIEW_PT2254_DATA;
 btwrite(data, BT848_GPIO_DATA);
 udelay(10);
 data &= ~WINVIEW_PT2254_STROBE;
 btwrite(data, BT848_GPIO_DATA);
}

/* ----------------------------------------------------------------------- */
/* mono/stereo control for various cards (which don't use i2c chips but    */
/* connect something to the GPIO pins                                      */
//------------------------------ gvbctv3pci_audio ------------------------------
static void gvbctv3pci_audio(struct bttv *btv,struct video_audio *v,int set)
{
 unsigned int con = 0;
 if (set)
 {
  btor(0x300, BT848_GPIO_OUT_EN);
  if (v->mode & VIDEO_SOUND_LANG1)
   con = 0x000;
  if (v->mode & VIDEO_SOUND_LANG2)
   con = 0x300;
  if (v->mode & VIDEO_SOUND_STEREO)
   con = 0x200;
//  if (v->mode & VIDEO_SOUND_MONO)
//   con = 0x100;
  btaor(con, ~0x300, BT848_GPIO_DATA);
 }
 else
 {
  v->mode = VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1  | VIDEO_SOUND_LANG2;
 }
}

/*
 * Mario Medina Nussbaum <medisoft@alohabbs.org.mx>
 *  I discover that on BT848_GPIO_DATA address a byte 0xcce enable stereo,
 *  0xdde enables mono and 0xccd enables sap
 *
 * Petr Vandrovec <VANDROVE@vc.cvut.cz>
 *  P.S.: At least mask in line above is wrong - GPIO pins 3,2 select
 *  input/output sound connection, so both must be set for output mode.
 *
 * Looks like it's needed only for the "tvphone", the "tvphone 98"
 * handles this with a tda9840
 *
 */
//-------------------------- avermedia_tvphone_audio ---------------------------
static void avermedia_tvphone_audio(struct bttv *btv,struct video_audio *v
                                    ,int set)
{
 int val = 0;
 if (set)
 {
  if (v->mode & VIDEO_SOUND_LANG1)   /* SAP */
   val = 0x02;
  if (v->mode & VIDEO_SOUND_STEREO)
   val = 0x01;
  if (val) {
   btaor(val, ~0x03, BT848_GPIO_DATA);
   if (bttv_gpio)
    bttv_gpio_tracking(btv,"avermedia");
  }
 }
 else
 {
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1;
  return;
 }
}

//------------------------- avermedia_tv_stereo_audio --------------------------
static void avermedia_tv_stereo_audio(struct bttv *btv, struct video_audio *v, int set)
{
 int val = 0;
 if (set)
 {
  if (v->mode & VIDEO_SOUND_LANG2)   /* SAP */
   val = 0x01;
  if (v->mode & VIDEO_SOUND_STEREO)  /* STEREO */
   val = 0x02;
  btaor(val, ~0x03, BT848_GPIO_DATA);
  if (bttv_gpio)
   bttv_gpio_tracking(btv,"avermedia");
 }
 else
 {
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
  return;
 }
}

/* Lifetec 9415 handling */
//-------------------------------- lt9415_audio --------------------------------
static void lt9415_audio(struct bttv *btv,struct video_audio *v,int set)
{
 int val = 0;
 if (btread(BT848_GPIO_DATA) & 0x4000)
 {
  v->mode = VIDEO_SOUND_MONO;
  return;
 }
 if (set)
 {
  if (v->mode & VIDEO_SOUND_LANG2)  /* A2 SAP */
   val = 0x0080;
  if (v->mode & VIDEO_SOUND_STEREO) /* A2 stereo */
   val = 0x0880;
  if ((v->mode & VIDEO_SOUND_LANG1) ||
      (v->mode & VIDEO_SOUND_MONO))
   val = 0;
  btaor(val, ~0x0880, BT848_GPIO_DATA);
  if (bttv_gpio)
   bttv_gpio_tracking(btv,"lt9415");
 }
 else
 {
  /* autodetect doesn't work with this card :-( */
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
  return;
 }
}

// TDA9821 on TerraTV+ Bt848, Bt878
//------------------------------- terratv_audio --------------------------------
static void terratv_audio(struct bttv *btv,struct video_audio *v,int set)
{
 unsigned int con = 0;
 if (set)
 {
  btor(0x180000, BT848_GPIO_OUT_EN);
  if (v->mode & VIDEO_SOUND_LANG2)
   con = 0x080000;
  if (v->mode & VIDEO_SOUND_STEREO)
   con = 0x180000;
  btaor(con, ~0x180000, BT848_GPIO_DATA);
  if (bttv_gpio)
   bttv_gpio_tracking(btv,"terratv");
 }
 else
 {
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
 }
}

//----------------------------- winfast2000_audio ------------------------------
static void winfast2000_audio(struct bttv *btv,struct video_audio *v,int set)
{
 unsigned long val = 0;
 if (set)
 {
  /*btor (0xc32000, BT848_GPIO_OUT_EN);*/
  if (v->mode & VIDEO_SOUND_MONO)  /* Mono */
   val = 0x420000;
  if (v->mode & VIDEO_SOUND_LANG1) /* Mono */
   val = 0x420000;
  if (v->mode & VIDEO_SOUND_LANG2) /* SAP */
   val = 0x410000;
  if (v->mode & VIDEO_SOUND_STEREO) /* Stereo */
   val = 0x020000;
  if (val)
  {
   btaor(val, ~0x430000, BT848_GPIO_DATA);
   if (bttv_gpio)
    bttv_gpio_tracking(btv,"winfast2000");
  }
 }
 else
 {
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
 }
}

/*
 * Dariusz Kowalewski <darekk@automex.pl>
 * sound control for Prolink PV-BT878P+9B (PixelView PlayTV Pro FM+NICAM
 * revision 9B has on-board TDA9874A sound decoder).
 *
 * Note: There are card variants without tda9874a. Forcing the "stereo sound route"
 *       will mute this cards.
 */
//------------------------------ pvbt878p9b_audio ------------------------------
static void pvbt878p9b_audio(struct bttv *btv,struct video_audio *v,int set)
{
 unsigned int val = 0;
#if BTTV_VERSION_CODE > KERNEL_VERSION(0,8,0)
 if (btv->radio_user)
  return;
#else
 if (btv->radio_user)
  return;
#endif
 if (set)
 {
  if (v->mode & VIDEO_SOUND_MONO)
  {
   val = 0x01;
  }
  if ((v->mode & (VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2))
      || (v->mode & VIDEO_SOUND_STEREO))
  {
   val = 0x02;
  }
  if (val)
  {
   btaor(val, ~0x03, BT848_GPIO_DATA);
   if (bttv_gpio)
    bttv_gpio_tracking(btv,"pvbt878p9b");
  }
 }
 else
 {
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
 }
}

/*
 * Dariusz Kowalewski <darekk@automex.pl>
 * sound control for FlyVideo 2000S (with tda9874 decoder)
 * based on pvbt878p9b_audio() - this is not tested, please fix!!!
 */
//------------------------------- fv2000s_audio --------------------------------
static void fv2000s_audio(struct bttv *btv, struct video_audio *v, int set)
{
 unsigned int val = 0xffff;
#if BTTV_VERSION_CODE > KERNEL_VERSION(0,8,0)
 if (btv->radio_user)
  return;
#else
 if (btv->radio_user)
  return;
#endif
 if (set)
 {
  if (v->mode & VIDEO_SOUND_MONO)
  {
   val = 0x0000;
  }
  if ((v->mode & (VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2))
      || (v->mode & VIDEO_SOUND_STEREO))
  {
   val = 0x1080; //-dk-???: 0x0880, 0x0080, 0x1800 ...
  }
  if (val != 0xffff)
  {
   btaor(val, ~0x1800, BT848_GPIO_DATA);
   if (bttv_gpio)
    bttv_gpio_tracking(btv,"fv2000s");
  }
 }
 else
 {
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
 }
}

/*
 * sound control for Canopus WinDVR PCI
 * Masaki Suzuki <masaki@btree.org>
 */
//-------------------------------- windvr_audio --------------------------------
static void windvr_audio(struct bttv *btv, struct video_audio *v, int set)
{
 unsigned long val = 0;
 if (set)
 {
  if (v->mode & VIDEO_SOUND_MONO)
   val = 0x040000;
  if (v->mode & VIDEO_SOUND_LANG1)
   val = 0;
  if (v->mode & VIDEO_SOUND_LANG2)
   val = 0x100000;
  if (v->mode & VIDEO_SOUND_STEREO)
   val = 0;
  if (val)
  {
   btaor(val, ~0x140000, BT848_GPIO_DATA);
   if (bttv_gpio)
    bttv_gpio_tracking(btv,"windvr");
  }
 }
 else
 {
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1 | VIDEO_SOUND_LANG2;
 }
}

/*
 * sound control for AD-TVK503
 * Hiroshi Takekawa <sian@big.or.jp>
 */
//------------------------------- adtvk503_audio -------------------------------
static void adtvk503_audio(struct bttv *btv, struct video_audio *v, int set)
{
 unsigned int con = 0xffffff;
 //btaor(0x1e0000, ~0x1e0000, BT848_GPIO_OUT_EN);
 if (set)
 {
  //btor(***, BT848_GPIO_OUT_EN);
  if (v->mode & VIDEO_SOUND_LANG1)
   con = 0x00000000;
  if (v->mode & VIDEO_SOUND_LANG2)
   con = 0x00180000;
  if (v->mode & VIDEO_SOUND_STEREO)
   con = 0x00000000;
  if (v->mode & VIDEO_SOUND_MONO)
   con = 0x00060000;
  if (con != 0xffffff)
  {
   btaor(con, ~0x1e0000, BT848_GPIO_DATA);
   if (bttv_gpio)
    bttv_gpio_tracking(btv, "adtvk503");
  }
 }
 else
 {
  v->mode = VIDEO_SOUND_MONO | VIDEO_SOUND_STEREO |
            VIDEO_SOUND_LANG1  | VIDEO_SOUND_LANG2;
 }
}

/* RemoteVision MX (rv605) muxsel helper [Miguel Freitas]
 *
 * This is needed because rv605 don't use a normal multiplex, but a crosspoint
 * switch instead (CD22M3494E). This IC can have multiple active connections
 * between Xn (input) and Yn (output) pins. We need to clear any existing
 * connection prior to establish a new one, pulsing the STROBE pin.
 *
 * The board hardwire Y0 (xpoint) to MUX1 and MUXOUT to Yin.
 * GPIO pins are wired as:
 *  GPIO[0:3] - AX[0:3] (xpoint) - P1[0:3] (microcontroler)
 *  GPIO[4:6] - AY[0:2] (xpoint) - P1[4:6] (microcontroler)
 *  GPIO[7]   - DATA (xpoint)    - P1[7] (microcontroler)
 *  GPIO[8]   -                  - P3[5] (microcontroler)
 *  GPIO[9]   - RESET (xpoint)   - P3[6] (microcontroler)
 *  GPIO[10]  - STROBE (xpoint)  - P3[7] (microcontroler)
 *  GPINTR    -                  - P3[4] (microcontroler)
 *
 * The microcontroler is a 80C32 like. It should be possible to change xpoint
 * configuration either directly (as we are doing) or using the microcontroler
 * which is also wired to I2C interface. I have no further info on the
 * microcontroler features, one would need to disassembly the firmware.
 * note: the vendor refused to give any information on this product, all
 *       that stuff was found using a multimeter! :)
 */
//-------------------------------- rv605_muxsel --------------------------------
static void rv605_muxsel(struct bttv *btv, unsigned int input)
{
 /* reset all conections */
 btaor(0x200,~0x200, BT848_GPIO_DATA);
 mdelay(1);
 btaor(0x000,~0x200, BT848_GPIO_DATA);
 mdelay(1);
 /* create a new conection */
 btaor(0x080,~0x480, BT848_GPIO_DATA);
 btaor(0x480,~0x480, BT848_GPIO_DATA);
 mdelay(1);
 btaor(0x080,~0x480, BT848_GPIO_DATA);
 mdelay(1);
}

// The Grandtec X-Guard framegrabber card uses two Dual 4-channel
// video multiplexers to provide up to 16 video inputs. These
// multiplexers are controlled by the lower 8 GPIO pins of the
// bt878. The multiplexers probably Pericom PI5V331Q or similar.

// xxx0 is pin xxx of multiplexer U5,
// yyy1 is pin yyy of multiplexer U2

#define ENA0    0x01
#define ENB0    0x02
#define ENA1    0x04
#define ENB1    0x08

#define IN10    0x10
#define IN00    0x20
#define IN11    0x40
#define IN01    0x80

//------------------------------- xguard_muxsel --------------------------------
static void xguard_muxsel(struct bttv *btv, unsigned int input)
{
 static const int masks[] = {
                ENB0, ENB0|IN00, ENB0|IN10, ENB0|IN00|IN10,
                ENA0, ENA0|IN00, ENA0|IN10, ENA0|IN00|IN10,
                ENB1, ENB1|IN01, ENB1|IN11, ENB1|IN01|IN11,
                ENA1, ENA1|IN01, ENA1|IN11, ENA1|IN01|IN11,
 };
 btwrite(masks[input%16], BT848_GPIO_DATA);
}

/*
 * ivc120_muxsel [Added by Alan Garfield <alan@fromorbit.com>]
 *
 * The IVC120G security card has 4 i2c controlled TDA8540 matrix
 * swichers to provide 16 channels to MUX0. The TDA8540's have
 * 4 indepedant outputs and as such the IVC120G also has the
 * optional "Monitor Out" bus. This allows the card to be looking
 * at one input while the monitor is looking at another.
 *
 * Since I've couldn't be bothered figuring out how to add an
 * independant muxsel for the monitor bus, I've just set it to
 * whatever the card is looking at.
 *
 *  OUT0 of the TDA8540's is connected to MUX0         (0x03)
 *  OUT1 of the TDA8540's is connected to "Monitor Out"        (0x0C)
 *
 *  TDA8540_ALT3 IN0-3 = Channel 13 - 16       (0x03)
 *  TDA8540_ALT4 IN0-3 = Channel 1 - 4         (0x03)
 *  TDA8540_ALT5 IN0-3 = Channel 5 - 8         (0x03)
 *  TDA8540_ALT6 IN0-3 = Channel 9 - 12                (0x03)
 *
 */

/* All 7 possible sub-ids for the TDA8540 Matrix Switcher */
#define I2C_TDA8540        0x90
#define I2C_TDA8540_ALT1   0x92
#define I2C_TDA8540_ALT2   0x94
#define I2C_TDA8540_ALT3   0x96
#define I2C_TDA8540_ALT4   0x98
#define I2C_TDA8540_ALT5   0x9a
#define I2C_TDA8540_ALT6   0x9c

//------------------------------- ivc120_muxsel --------------------------------
static void ivc120_muxsel(struct bttv *btv, unsigned int input)
{
 // Simple maths
 int key = input % 4;
 int matrix = input / 4;
 CPK(dprintk("bttv%d: ivc120_muxsel: Input - %02d | TDA - %02d | In - %02d\n",
      btv->c.nr, input, matrix, key));
 // Handles the input selection on the TDA8540's
 bttv_I2CWrite(btv, I2C_TDA8540_ALT3, 0x00,
            ((matrix == 3) ? (key | key << 2) : 0x00), 1);
 bttv_I2CWrite(btv, I2C_TDA8540_ALT4, 0x00,
            ((matrix == 0) ? (key | key << 2) : 0x00), 1);
 bttv_I2CWrite(btv, I2C_TDA8540_ALT5, 0x00,
            ((matrix == 1) ? (key | key << 2) : 0x00), 1);
 bttv_I2CWrite(btv, I2C_TDA8540_ALT6, 0x00,
            ((matrix == 2) ? (key | key << 2) : 0x00), 1);

 // Handles the output enables on the TDA8540's
 bttv_I2CWrite(btv, I2C_TDA8540_ALT3, 0x02,
            ((matrix == 3) ? 0x03 : 0x00), 1);  // 13 - 16
 bttv_I2CWrite(btv, I2C_TDA8540_ALT4, 0x02,
            ((matrix == 0) ? 0x03 : 0x00), 1);  // 1-4
 bttv_I2CWrite(btv, I2C_TDA8540_ALT5, 0x02,
            ((matrix == 1) ? 0x03 : 0x00), 1);  // 5-8
 bttv_I2CWrite(btv, I2C_TDA8540_ALT6, 0x02,
            ((matrix == 2) ? 0x03 : 0x00), 1);  // 9-12

 // Selects MUX0 for input on the 878
 btaor((0)<<5, ~(3<<5), BT848_IFORM);
}

/* ----------------------------------------------------------------------- */
/* motherboard chipset specific stuff                                      */
//----------------------------- bttv_check_chipset -----------------------------
void __devinit bttv_check_chipset(void)
{
 int pcipci_fail = 0;
 struct pci_dev *dev = 0;
 if (pci_pci_problems & PCIPCI_FAIL)
  pcipci_fail = 1;
 if (pci_pci_problems & (PCIPCI_TRITON|PCIPCI_NATOMA|PCIPCI_VIAETBF))
  triton1 = 1;
 if (pci_pci_problems & PCIPCI_VSFX)
  vsfx = 1;
#ifdef PCIPCI_ALIMAGIK
 if (pci_pci_problems & PCIPCI_ALIMAGIK)
  latency = 0x0A;
#endif
#if 0
 /* print which chipset we have */
 while (0!=(dev = pci_find_class(PCI_CLASS_BRIDGE_HOST << 8,dev)))
  CPK(printk(KERN_INFO "bttv: Host bridge is %s\n",dev->name));
#endif
 /* print warnings about any quirks found */
 if (triton1)
  CPK(printk(KERN_INFO "bttv: Host bridge needs ETBF enabled.\n"));
 if (vsfx)
  CPK(printk(KERN_INFO "bttv: Host bridge needs VSFX enabled.\n"));
 if (pcipci_fail)
 {
  CPK(printk(KERN_WARNING "bttv: BT848 and your chipset may not work together.\n"));
  if (UNSET == no_overlay)
  {
   CPK(printk(KERN_WARNING "bttv: going to disable overlay.\n"));
   no_overlay = 1;
  }
 }
 if (UNSET != latency)
  CPK(printk(KERN_INFO "bttv: pci latency fixup [%d]\n",latency));
 while (0!=(dev = pci_find_device(PCI_VENDOR_ID_INTEL,
          PCI_DEVICE_ID_INTEL_82441, dev)))
 {
  unsigned char b;
  pci_read_config_byte(dev, 0x53, &b);
  if (bttv_debug)
   CPK(printk(KERN_INFO "bttv: Host bridge: 82441FX Natoma, "
          "bufcon=0x%02x\n",b));
 }
}

//---------------------------- bttv_handle_chipset -----------------------------
int __devinit bttv_handle_chipset(struct bttv *btv)
{
 unsigned char command;
 if (!triton1 && !vsfx && UNSET == latency)
  return 0;
 if (bttv_verbose)
 {
  if (triton1)
   CPK(printk(KERN_INFO "bttv%d: enabling ETBF (430FX/VP3 compatibilty)\n",btv->nr));
  if (vsfx && btv->id >= 878)
   CPK(printk(KERN_INFO "bttv%d: enabling VSFX\n",btv->nr));
  if (UNSET != latency)
   CPK(printk(KERN_INFO "bttv%d: setting pci timer to %d\n",
          btv->nr,latency));
 }
 if (btv->id < 878)
 {
  /* bt848 (mis)uses a bit in the irq mask for etbf */
  if (triton1)
   btv->triton1 = BT848_INT_ETBF;
 }
 else
 {
  /* bt878 has a bit in the pci config space for it */
  pci_read_config_byte(btv->dev, BT878_DEVCTRL, &command);
  if (triton1)
   command |= BT878_EN_TBFX;
  if (vsfx)
   command |= BT878_EN_VSFX;
  pci_write_config_byte(btv->dev, BT878_DEVCTRL, command);
 }
 if (UNSET != latency)
  pci_write_config_byte(btv->dev, PCI_LATENCY_TIMER, (unsigned char)latency);
 return 0;
}

#ifdef __cplusplus
}
#endif
