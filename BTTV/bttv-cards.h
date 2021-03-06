/* $Id: bttv-cards.h,v 1.12 2004/01/29 09:10:11 smilcke Exp $ */

/*
 * bttv-cards.h
 * Autor:               Stefan Milcke (Automatic generation from CnvBttv.CMD)
 * Erstellt am:         27.01.2004
 * Letzte Aenderung am: 27.01.2004
 *
*/

#include <media/tuner.h>
#include "bttv.h"
#include "bttvp.h"

#ifndef NO_EXTERN_BTTV_TVCARDS
//extern struct tvcard bttv_tvcards[];
//extern const int bttv_num_tvcards;
#else
struct tvcard bttv_tvcards[] = {
{
/* ---- card 0x00 ---------------------------------- */
 " *** UNKNOWN/GENERIC *** ",                // name
 4,                                          // video_inputs
 1,                                          // audio_inputs
 0,                                          // tuner
 2,                                          // svhs
 0,                                          // digital_mode
 0,                                          // gpiomask
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},          // muxsel
 {0,0,0,0,0,0},                              // audiomux
 0,                                          // gpiomask2
 0,                                          // no_msp24xx
 0,                                          // no_tda9875
 0,                                          // no_tda7432
 0,                                          // needs_tvaudio
 0,                                          // msp34xx_alt
 0,                                          // no_video
 0,                                          // has_dvb
 0,                                          // has_remote
 0,                                          // no_gpioirq
 PLL_NONE,                                   // pll
 -1,                                         // tuner_type
 0,                                          // has_radio
 0,                                          // audio_hook
 0,                                          // muxsel_hook
},{
 "MIRO PCTV",
 4,
 1,
 0,
 2,
 0,
 15,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {2,0,0,0,10,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
 "Hauppauge (bt848)",
 4,
 1,
 0,
 2,
 0,
 7,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,2,3,4,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
 "STB, Gateway P/N 6000699 (bt848)",
 3,
 1,
 0,
 2,
 0,
 7,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {4,0,2,3,1,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_NTSC,
 0,
 0,
 0,
},{
/* ---- card 0x04 ---------------------------------- */
 "Intel Create and Share PCI/ Smart Video Recorder III",
 4,
 0,
 -1,
 2,
 0,
 0,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 4,
 0,
 0,
 0,
},{
 "Diamond DTV2000",
 4,
 1,
 0,
 2,
 0,
 3,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,0,1,3,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
/*                0x04 for some cards ?? */
 "AVerMedia TVPhone",
 3,
 1,
 0,
 3,
 0,
 0x0f,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x0c,0x04,0x08,0x04,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 1,
 0,
 PLL_NONE,
 -1,
 0,
 avermedia_tvphone_audio,
 0,
},{
 "MATRIX-Vision MV-Delta",
 5,
 1,
 -1,
 3,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x08 ---------------------------------- */
 "Lifeview FlyVideo II (Bt848) LR26 / MAXI TV Video PCI2 LR26",
 4,
 1,
 0,
 2,
 0,
 0xc00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0xc00,0x800,0x400,0xc00,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "IMS/IXmicro TurboTV",
 3,
 1,
 0,
 2,
 0,
 3,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {1,1,2,3,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_TEMIC_PAL,
 0,
 0,
 0,
},{
 "Hauppauge (bt878)",
 4,
 1,
 0,
 2,
 0,
 0x0f, /* old: 7 */
 {2,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,2,3,4,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "MIRO PCTV pro",
 3,
 1,
 0,
 2,
 0,
 0x3014f,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x20001,0x10001,0,0,10,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x0c ---------------------------------- */
 "ADS Technologies Channel Surfer TV (bt848)",
 3,
 1,
 0,
 2,
 0,
 15,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {13,14,11,7,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
 "AVerMedia TVCapture 98",
 3,
 4,
 0,
 2,
 0,
 15,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {13,14,11,7,0,0},
 0,
 0,
 0,
 0,
 1,
 1,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL,
 0,
 avermedia_tv_stereo_audio,
 0,
},{
 "Aimslab Video Highway Xtreme (VHX)",
 3,
 1,
 0,
 2,
 0,
 7,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,2,1,3,4,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Zoltrix TV-Max",
 3,
 1,
 0,
 2,
 0,
 15,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,1,0,10,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x10 ---------------------------------- */
 "Prolink Pixelview PlayTV (bt878)",
 3,
 1,
 0,
 2,
 0,
 0x01fe00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x001e00,0,0x018000,0x014000,0x002000,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Leadtek WinView 601",
 3,
 1,
 0,
 2,
 0,
 0x8300f8,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x4fa007,0xcfa007,0xcfa007,0xcfa007,0xcfa007,0xcfa007},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 winview_audio,
 0,
},{
 "AVEC Intercapture",
 3,
 2,
 0,
 2,
 0,
 0,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {1,0,0,0,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
 "Lifeview FlyVideo II EZ /FlyKit LR38 Bt848 (capture only)",
 4,
 1,
 -1,
 -1,
 0,
 0x8dff00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x14 ---------------------------------- */
 "CEI Raffles Card",
 3,
 3,
 0,
 2,
 0,
 0,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
 "Lifeview FlyVideo 98/ Lucky Star Image World ConferenceTV LR50",
 4,
 2,  // tuner, line in
 0,
 2,
 0,
 0x1800,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0x800,0x1000,0x1000,0x1800,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL_I,
 0,
 0,
 0,
},{
 "Askey CPH050/ Phoebe Tv Master + FM",
 3,
 1,
 0,
 2,
 0,
 0xc00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,0x800,0x400,0xc00,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Modular Technology MM201/MM202/MM205/MM210/MM215 PCTV, bt878",
 3,
 1,
 0,
 -1,
 0,
 7,
 {2,3,-1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_ALPS_TSBB5_PAL_I,
 0,
 0,
 0,
},{
/* ---- card 0x18 ---------------------------------- */
 "Askey CPH05X/06X (bt878) [many vendors]",
 3,
 1,
 0,
 2,
 0,
 0xe00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x400,0x400,0x400,0x400,0xc00,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Terratec TerraTV+ Version 1.0 (Bt848)/ Terra TValue Version 1.0/ Vobis TV-Boostar",
 3,
 1,
 0,
 2,
 0,
 0x1f0fff,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x20000,0x30000,0x10000,0,0x40000,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 TUNER_PHILIPS_PAL,
 0,
 terratv_audio,
 0,
},{
 "Hauppauge WinCam newer (bt878)",
 4,
 1,
 0,
 3,
 0,
 7,
 {2,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,2,3,4,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
 "Lifeview FlyVideo 98/ MAXI TV Video PCI2 LR50",
 4,
 2,
 0,
 2,
 0,
 0x1800,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0x800,0x1000,0x1000,0x1800,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_SECAM,
 0,
 0,
 0,
},{
/* ---- card 0x1c ---------------------------------- */
/* GPIO wiring:
External 20 pin connector (for Active Radio Upgrade board)
gpio00: i2c-sda
gpio01: i2c-scl
gpio02: om5610-data
gpio03: om5610-clk
gpio04: om5610-wre
gpio05: om5610-stereo
gpio06: rds6588-davn
gpio07: Pin 7 n.c.
gpio08: nIOW
gpio09+10: nIOR, nSEL ?? (bt878)
gpio09: nIOR (bt848)
gpio10: nSEL (bt848)
Sound Routing:
gpio16: u2-A0 (1st 4052bt)
gpio17: u2-A1
gpio18: u2-nEN
gpio19: u4-A0 (2nd 4052)
gpio20: u4-A1
u4-nEN - GND
Btspy:
00000 : Cdrom (internal audio input)
10000 : ext. Video audio input
20000 : TV Mono
a0000 : TV Mono/2
1a0000 : TV Stereo
30000 : Radio
40000 : Mute
*/
 "Terratec TerraTV+ Version 1.1 (bt878)",
 3,
 1,
 0,
 2,
 0,
 0x1f0fff,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x20000,0x30000,0x10000,0x00000,0x40000,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 TUNER_PHILIPS_PAL,
 0,
 terratv_audio,
 0,
},{
/* Jannik Fritsch <jannik@techfak.uni-bielefeld.de> */
 "Imagenation PXC200",
 5,
 1,
 -1,
 1, /* was: 4 */
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
 "Lifeview FlyVideo 98 LR50",
 4,
 1,
 0,
 2,
 0,
 0x1800,  //0x8dfe00
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0x0800,0x1000,0x1000,0x1800,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Formac iProTV, Formac ProTV I (bt848)",
 4,
 1,
 0,
 3,
 0,
 1,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {1,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL,
 0,
 0,
 0,
},{
/* ---- card 0x20 ---------------------------------- */
 "Intel Create and Share PCI/ Smart Video Recorder III",
 4,
 0,
 -1,
 2,
 0,
 0,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 4,
 0,
 0,
 0,
},{
 "Terratec TerraTValue Version Bt878",
 3,
 1,
 0,
 2,
 0,
 0xffff00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x500,0,0x300,0x900,0x900,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL,
 0,
 0,
 0,
},{
/* Audio Routing for "WinFast 2000 XP" (no tv stereo !)
gpio23 -- hef4052:nEnable (0x800000)
gpio12 -- hef4052:A1
gpio13 -- hef4052:A0
0x0000: external audio
0x1000: FM
0x2000: TV
0x3000: n.c.
Note: There exists another variant "Winfast 2000" with tv stereo !?
Note: eeprom only contains FF and pci subsystem id 107d:6606
*/
 "Leadtek WinFast 2000/ WinFast 2000 XP",
 4,
 1,
 0,
 2,
 0,
 0xc33000,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x422000,0x1000,0x0000,0x620000,0x800000,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 1,
 0,
 PLL_28,
 5, // default for now, gpio reads BFFF06 for Pal bg+dk
 0,
 winfast2000_audio,
 0,
},{
 "Lifeview FlyVideo 98 LR50 / Chronos Video Shuttle II",
 4,
 3,
 0,
 2,
 0,
 0x1800,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0x800,0x1000,0x1000,0x1800,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x24 ---------------------------------- */
 "Lifeview FlyVideo 98FM LR50 / Typhoon TView TV/FM Tuner",
 4,
 3,
 0,
 2,
 0,
 0x1800,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0x800,0x1000,0x1000,0x1800,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Prolink PixelView PlayTV pro",
 3,
 1,
 0,
 2,
 0,
 0xff,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x21,0x20,0x24,0x2c,0x29,0x29},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Askey CPH06X TView99",
 4,
 1,
 0,
 2,
 0,
 0x551e00,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x551400,0x551200,0,0,0x551c00,0x551200},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 1,
 0,
 0,
 0,
},{
 "Pinnacle PCTV Studio/Rave",
 3,
 1,
 0,
 2,
 0,
 0x03000F,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {2,0,0,0,1,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x28 ---------------------------------- */
 "STB TV PCI FM, Gateway P/N 6000704 (bt878), 3Dfx VoodooTV 100",
 3,
 1,
 0,
 2,
 0,
 7,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {4,0,2,3,1,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_NTSC,
 0,
 0,
 0,
},{
 "AVerMedia TVPhone 98",
 3,
 4,
 0,
 2,
 0,
 15,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {13,4,11,7,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 avermedia_tvphone_audio,
 0,
},{
 "ProVideo PV951", /* pic16c54 */
 3,
 1,
 0,
 2,
 0,
 0,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 1,
 0,
 0,
 0,
},{
 "Little OnAir TV",
 3,
 1,
 0,
 2,
 0,
 0xe00b,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0xff9ff6,0xff9ff6,0xff1ff7,0,0xff3ffc,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x2c ---------------------------------- */
 "Sigma TVII-FM",
 2,
 1,
 0,
 -1,
 0,
 3,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {1,1,0,2,3,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
 "MATRIX-Vision MV-Delta 2",
 5,
 1,
 -1,
 3,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Zoltrix Genie TV/FM",
 3,
 1,
 0,
 2,
 0,
 0xbcf03f,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0xbc803f,0xbc903f,0xbcb03f,0,0xbcb03f,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 21,
 0,
 0,
 0,
},{
 "Terratec TV/Radio+",
 3,
 1,
 0,
 2,
 0,
 0x70000,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x20000,0x30000,0x10000,0,0x40000,0x20000},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_35,
 1,
 0,
 0,
 0,
},{
/* ---- card 0x30 ---------------------------------- */
 "Askey CPH03x/ Dynalink Magic TView",
 3,
 1,
 0,
 2,
 0,
 15,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {2,0,0,0,1,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "IODATA GV-BCTV3/PCI",
 3,
 1,
 0,
 2,
 0,
 0x010f00,
 {2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x10000,0,0x10000,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_ALPS_TSHC6_NTSC,
 0,
 gvbctv3pci_audio,
 0,
},{
/* GPIO wiring: (different from Rev.4C !)
GPIO17: U4.A0 (first hef4052bt)
GPIO19: U4.A1
GPIO20: U5.A1 (second hef4052bt)
GPIO21: U4.nEN
GPIO22: BT832 Reset Line
GPIO23: A5,A0, U5,nEN
Note: At i2c=0x8a is a Bt832 chip, which changes to 0x88 after being reset via GPIO22
*/
 "Prolink PV-BT878P+4E / PixelView PlayTV PAK / Lenco MXTV-9578 CP",
 5,
 1,
 0,
 3,
 0,
 0xAA0000,
 {2,3,1,1,-1,0,0,0,0,0,0,0,0,0,0,0},
 {0x20000,0,0x80000,0x80000,0xa8000,0x46000},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL_I,
 0,
 0,
 0,
},{
 "Eagle Wireless Capricorn2 (bt878A)",
 4,
 1,
 0,
 2,
 0,
 7,
 {2,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,2,3,4,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1 /* TUNER_ALPS_TMDH2_NTSC */,
 0,
 0,
 0,
},{
/* ---- card 0x34 ---------------------------------- */
/* David Härdeman <david@2gen.com> */
/* sound path (5 sources):
MUX1 (mask 0x03), Enable Pin 0x08 (0=enable, 1=disable)
0= ext. Audio IN
1= from MUX2
2= Mono TV sound from Tuner
3= not connected
MUX2 (mask 0x30000):
0,2,3= from MSP34xx
1= FM stereo Radio from Tuner */
 "Pinnacle PCTV Studio Pro",
 4,
 1,
 0,
 3,
 0,
 0x03000F,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {1,0xd0001,0,0,10,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* Claas Langbehn <claas@bigfoot.com>,
Sven Grothklags <sven@upb.de> */
 "Typhoon TView RDS + FM Stereo / KNC1 TV Station RDS",
 3,
 3,
 0,
 2,
 0,
 0x1c,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0x10,8,4,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL_I,
 0,
 0,
 0,
},{
/* Tim Röstermundt <rosterm@uni-muenster.de>
in de.comp.os.unix.linux.hardware:
options bttv card=0 pll=1 radio=1 gpiomask=0x18e0
audiomux=0x44c71f,0x44d71f,0,0x44d71f,0x44dfff
options tuner type=5 */
/* For cards with tda9820/tda9821:
0x0000: Tuner normal stereo
0x0080: Tuner A2 SAP (second audio program = Zweikanalton)
0x0880: Tuner A2 stereo */
 "Lifeview FlyVideo 2000 /FlyVideo A2/ Lifetec LT 9415 TV [LR90]",
 4,
 1,
 0,
 2,
 0,
 0x18e0,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x0000,0x0800,0x1000,0x1000,0x18e0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* Miguel Angel Alvarez <maacruz@navegalia.com>
old Easy TV BT848 version (model CPH031) */
 "Askey CPH031/ BESTBUY Easy TV",
 4,
 1,
 0,
 2,
 0,
 0xF,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {2,0,0,0,10,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_TEMIC_PAL,
 0,
 0,
 0,
},{
/* ---- card 0x38 ---------------------------------- */
/* Gordon Heydon <gjheydon@bigfoot.com ('98) */
 "Lifeview FlyVideo 98FM LR50",
 4,
 3,
 0,
 2,
 0,
 0x1800,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0x800,0x1000,0x1000,0x1800,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 5,
 0,
 0,
 0,
},{
/* This is the ultimate cheapo capture card
* just a BT848A on a small PCB!
* Steve Hosgood <steve@equiinet.com> */
 "GrandTec 'Grand Video Capture' (Bt848)",
 2,
 0,
 -1,
 1,
 0,
 0,
 {3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_35,
 -1,
 0,
 0,
 0,
},{
/* Daniel Herrington <daniel.herrington@home.com> */
 "Askey CPH060/ Phoebe TV Master Only (No FM)",
 3,
 1,
 0,
 2,
 0,
 0xe00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x400,0x400,0x400,0x400,0x800,0x400},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_TEMIC_4036FY5_NTSC,
 0,
 0,
 0,
},{
/* Matti Mottus <mottus@physic.ut.ee> */
 "Askey CPH03x TV Capturer",
 4,
 1,
 0,
 2,
 0,
 0x03000F,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {2,0,0,0,1,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 0,
 0,
 0,
 0,
},{
/* ---- card 0x3c ---------------------------------- */
/* Philip Blundell <philb@gnu.org> */
 "Modular Technology MM100PCTV",
 2,
 2,
 0,
 -1,
 0,
 11,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {2,0,0,1,8,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_35,
 TUNER_TEMIC_PAL,
 0,
 0,
 0,
},{
/* Adrian Cox <adrian@humboldt.co.uk */
 "AG Electronics GMV1",
 2,
 0,
 -1,
 1,
 0,
 0xF,
 {2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* Miguel Angel Alvarez <maacruz@navegalia.com>
new Easy TV BT878 version (model CPH061)
special thanks to Informatica Mieres for providing the card */
 "Askey CPH061/ BESTBUY Easy TV (bt878)",
 3,
 2,
 0,
 2,
 0,
 0xFF,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {1,0,4,4,9,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL,
 0,
 0,
 0,
},{
/* Lukas Gebauer <geby@volny.cz> */
 "ATI TV-Wonder",
 3,
 1,
 0,
 2,
 0,
 0xf03f,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0xbffe,0,0xbfff,0,0xbffe,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_TEMIC_4006FN5_MULTI_PAL,
 0,
 0,
 0,
},{
/* ---- card 0x40 ---------------------------------- */
/* Lukas Gebauer <geby@volny.cz> */
 "ATI TV-Wonder VE",
 2,
 1,
 0,
 -1,
 0,
 1,
 {2,3,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,1,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_TEMIC_4006FN5_MULTI_PAL,
 0,
 0,
 0,
},{
/* DeeJay <deejay@westel900.net (2000S) */
/* Radio changed from 1e80 to 0x800 to make
FlyVideo2000S in .hu happy (gm)*/
/* -dk-???: set mute=0x1800 for tda9874h daughterboard */
 "Lifeview FlyVideo 2000S LR90",
 3,
 3,
 0,
 2,
 0,
 0x18e0,
 {2,3,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x0000,0x0800,0x1000,0x1000,0x1800,0x1080},
 0,
 0,
 1,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 5,
 0,
 fv2000s_audio,
 0,
},{
 "Terratec TValueRadio",
 3,
 1,
 0,
 2,
 0,
 0xffff00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x500,0x500,0x300,0x900,0x900,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL,
 0,
 0,
 0,
},{
/* TANAKA Kei <peg00625@nifty.com> */
 "IODATA GV-BCTV4/PCI",
 3,
 1,
 0,
 2,
 0,
 0x010f00,
 {2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x10000,0,0x10000,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_SHARP_2U5JF5540_NTSC,
 0,
 gvbctv3pci_audio,
 0,
},{
/* ---- card 0x44 ---------------------------------- */
/* tuner, Composit, SVid, Composit-on-Svid-adapter*/
 "3Dfx VoodooTV FM (Euro), VoodooTV 200 (USA)",
 4,
 1,
 0,
 -1,
 0,
 0x4f8a00,
 {2,3,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x947fff,0x987fff,0x947fff,0x947fff,0x947fff,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_MT2032,
 0,
 0,
 0,
},{
/* Philip Blundell <pb@nexus.co.uk> */
 "Active Imaging AIMMS",
 1,
 0,
 -1,
 0,
 0,
 0,
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* Tomasz Pyra <hellfire@sedez.iq.pl> */
/* GPIO wiring:
GPIO0: U4.A0 (hef4052bt)
GPIO1: U4.A1
GPIO2: U4.A1 (second hef4052bt)
GPIO3: U4.nEN, U5.A0, A5.nEN
GPIO8-15: vrd866b ?
*/
 "Prolink Pixelview PV-BT878P+ (Rev.4C,8E)",
 3,
 4,
 0,
 2,
 0,
 15,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,11,7,13,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 1,
 0,
 PLL_28,
 25,
 0,
 0,
 0,
},{
 "Lifeview FlyVideo 98EZ (capture only) LR51",
 4,
 0,
 -1,
 2,
 0,
 0,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 0,
 0,
 0,
 0,
},{
/* ---- card 0x48 ---------------------------------- */
/* Dariusz Kowalewski <darekk@automex.pl> */
/* GPIO wiring:
GPIO0: A0 hef4052
GPIO1: A1 hef4052
GPIO3: nEN hef4052
GPIO8-15: vrd866b
GPIO20,22,23: R30,R29,R28
*/
 "Prolink Pixelview PV-BT878P+9B (PlayTV Pro rev.9B FM+NICAM)",
 4,
 1,
 0,
 2,
 0,
 0x3f,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x01,0x00,0x03,0x03,0x09,0x02},
 0,
 0,
 1,
 0,
 1,
 0,
 0,
 0,
 1,
 0,
 PLL_28,
 5,
 0,
 pvbt878p9b_audio, // Note: not all cards have stereo
 0,
},{
/* Clay Kunz <ckunz@mail.arc.nasa.gov> */
/* you must jumper JP5 for the card to work */
 "Sensoray 311",
 5,
 0,
 -1,
 4,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 0,
},{
/* Miguel Freitas <miguel@cetuc.puc-rio.br> */
 "RemoteVision MX (RV605)",
 16,
 0,
 -1,
 -1,
 0,
 0x00,
 {0x33,0x13,0x23,0x43,0xf3,0x73,0xe3,0x03,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0x07ff,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 -1,
 0,
 0,
 rv605_muxsel,
},{
 "Powercolor MTV878/ MTV878R/ MTV878F",
 3,
 2,
 0,
 2,
 0,
 0x1C800F,  // Bit0-2: Audio select, 8-12:remote control 14:remote valid 15:remote reset
 {2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,2,2,4,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL,
 0,
 0,
 0,
},{
/* ---- card 0x4c ---------------------------------- */
/* Masaki Suzuki <masaki@btree.org> */
 "Canopus WinDVR PCI (COMPAQ Presario 3524JP, 5112JP)",
 3,
 1,
 0,
 2,
 0,
 0x140007,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,2,3,4,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 TUNER_PHILIPS_NTSC,
 0,
 windvr_audio,
 0,
},{
 "GrandTec Multi Capture Card (Bt878)",
 4,
 0,
 -1,
 -1,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* GPIO Info:
GPIO0,1:   HEF4052 A0,A1
GPIO2:     HEF4052 nENABLE
GPIO3-7:   n.c.
GPIO8-13:  IRDC357 data0-5 (data6 n.c. ?) [chip not present on my card]
GPIO14,15: ??
GPIO16-21: n.c.
GPIO22,23: ??
??       : mtu8b56ep microcontroller for IR (GPIO wiring unknown)*/
 "Jetway TV/Capture JW-TV878-FBK, Kworld KW-TV878RF",
 4,
 3,
 0,
 2,
 0,
 7,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,4,4,4,4},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 5, // Samsung TCPA9095PC27A (BG+DK), philips compatible, w/FM, stereo and
 0,
 0,
 0,
},{
/* Arthur Tetzlaff-Deas, DSP Design Ltd <software@dspdesign.com> */
 "DSP Design TCVIDEO",
 4,
 0,
 0,
 -1,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x50 ---------------------------------- */
 "Hauppauge WinTV PVR",
 4,
 1,
 0,
 2,
 0,
 7,
 {2,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {7,0,0,0,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "IODATA GV-BCTV5/PCI",
 3,
 1,
 0,
 2,
 0,
 0x0f0f80,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x030000,0x010000,0x030000,0,0x020000,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_NTSC_M,
 0,
 gvbctv3pci_audio,
 0,
},{
 "Osprey 100/150 (878)", /* 0x1(2|3)-45C6-C1 */
 4,                  /* id-inputs-clock */
 0,
 -1,
 3,
 0,
 0,
 {3,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Osprey 100/150 (848)", /* 0x04-54C0-C1 & older boards */
 3,
 0,
 -1,
 2,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x54 ---------------------------------- */
 "Osprey 101 (848)", /* 0x05-40C0-C1 */
 2,
 0,
 -1,
 1,
 0,
 0,
 {3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Osprey 101/151",       /* 0x1(4|5)-0004-C4 */
 1,
 0,
 -1,
 -1,
 0,
 0,
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Osprey 101/151 w/ svid",  /* 0x(16|17|20)-00C4-C1 */
 2,
 0,
 -1,
 1,
 0,
 0,
 {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Osprey 200/201/250/251",  /* 0x1(8|9|E|F)-0004-C4 */
 1,
 1,
 -1,
 -1,
 0,
 0,
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x58 ---------------------------------- */
 "Osprey 200/250",   /* 0x1(A|B)-00C4-C1 */
 2,
 1,
 -1,
 1,
 0,
 0,
 {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Osprey 210/220",   /* 0x1(A|B)-04C0-C1 */
 2,
 1,
 -1,
 1,
 0,
 0,
 {2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Osprey 500",   /* 500 */
 2,
 1,
 -1,
 1,
 0,
 0,
 {2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Osprey 540",   /* 540 */
 4,
 1,
 -1,
 0,
 0,
 0,
 {2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0
},{
/* ---- card 0x5C ---------------------------------- */
 "Osprey 2000",  /* 2000 */
 2,
 1,
 -1,
 1,
 0,
 0,
 {2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* M G Berberich <berberic@forwiss.uni-passau.de> */
 "IDS Eagle",
 4,
 0,
 -1,
 -1,
 0,
 0,
 {0,1,2,3,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 eagle_muxsel,
},{
 "Pinnacle PCTV Sat",
 2,
 0,
 -1,
 1,
 0,
 0x01,
 {3,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,1,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* sound routing:
GPIO=0x00,0x01,0x03: mute (?)
0x02: both TV and radio (tuner: FM1216/I)
The card has onboard audio connectors labeled "cdrom" and "board",
not soldered here, though unknown wiring.
Card lacks: external audio in, pci subsystem id.
*/
 "Formac ProTV II (bt878)",
 4,
 1,
 0,
 3,
 0,
 2,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {2,2,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 TUNER_PHILIPS_PAL,
 0,
 0,
 0,
},{
/* ---- card 0x60 ---------------------------------- */
 "MachTV",
 3,
 1,
 0,
 -1,
 0,
 7,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,1,2,3,4,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 1,
 5,
 0,
 0,
 0,
},{
 "Euresys Picolo",
 3,
 0,
 -1,
 2,
 0,
 0,
 {2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 0,
 0,
 0,
 0,
},{
/* Luc Van Hoeylandt <luc@e-magic.be> */
 "ProVideo PV150", /* 0x4f */
 2,
 0,
 -1,
 -1,
 0,
 0,
 {2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* Hiroshi Takekawa <sian@big.or.jp> */
/* This card lacks subsystem ID */
/*                  Tuner, Radio, external, internal, off,  on */
 "AD-TVK503", /* 0x63 */
 4,
 1,
 0,
 2,
 0,
 0x001e8007,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0x08,0x0f,0x0a,0x08,0x0f,0x08},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 2,
 0,
 adtvk503_audio,
 0,
},{
/* ---- card 0x64 ---------------------------------- */
/* Notes:
- card lacks subsystem ID
- stereo variant w/ daughter board with tda9874a @0xb0
- Audio Routing:
always from tda9874 independent of GPIO (?)
external line in: unknown
- Other chips: em78p156elp @ 0x96 (probably IR remote control)
hef4053 (instead 4052) for unknown function
*/
 "Hercules Smart TV Stereo",
 4,
 1,
 0,
 2,
 0,
 0x00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 5,
 0,
 0,
 0,
},{
/* Bt878, Bt832, FI1246 tuner; no pci subsystem id
only internal line out: (4pin header) RGGL
Radio must be decoded by msp3410d (not routed through)*/
 "Pace TV & Radio Card",
 4,
 1,
 0,
 2,
 0,
 0,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 1,
 0,
 0,
 0,
},{
/* Chris Willing <chris@vislab.usyd.edu.au> */
 "IVC-200",
 1,
 0,
 -1,
 -1,
 0,
 0xdf,
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "Grand X-Guard / Trust 814PCI",
 16,
 0,
 -1,
 -1,
 0,
 0,
 {2,2,2,2,3,3,3,3,1,1,1,1,0,0,0,0},
 {0,0,0,0,0,0},
 0xff,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 4,
 0,
 0,
 xguard_muxsel,
},{
/* ---- card 0x68 ---------------------------------- */
 "Nebula Electronics DigiTV",
 0,
 0,
 0,
 -1,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 1,
 0,
 1,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* Jorge Boncompte - DTI2 <jorge@dti2.net> */
 "ProVideo PV143",
 4,
 0,
 -1,
 -1,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* M.Klahr@phytec.de */
 "PHYTEC VD-009-X1 MiniDIN (bt878)",
 4,
 0,
 -1, /* card has no tuner */
 3,
 0,
 0x00,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "PHYTEC VD-009-X1 Combi (bt878)",
 4,
 0,
 -1, /* card has no tuner */
 3,
 0,
 0x00,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* ---- card 0x6c ---------------------------------- */
 "PHYTEC VD-009 MiniDIN (bt878)",
 10,
 0,
 -1, /* card has no tuner */
 9,
 0,
 0x00,
 {0x02,0x12,0x22,0x32,0x03,0x13,0x23,0x33,0x01,0x00,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0x03, /* gpiomask2 defines the bits used to switch audio */
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "PHYTEC VD-009 Combi (bt878)",
 10,
 0,
 -1, /* card has no tuner */
 9,
 0,
 0x00,
 {0x02,0x12,0x22,0x32,0x03,0x13,0x23,0x33,0x01,0x01,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0x03, /* gpiomask2 defines the bits used to switch audio */
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
 "IVC-100",
 4,
 0,
 -1,
 -1,
 0,
 0xdf,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 0,
},{
/* IVC-120G - Alan Garfield <alan@fromorbit.com> */
 "IVC-120G",
 16,
 0,    /* card has no audio */
 -1,   /* card has no tuner */
 -1,   /* card has no svhs */
 0,
 0x00,
 {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_28,
 -1,
 0,
 0,
 ivc120_muxsel,
},{
/* ---- card 0x70 ---------------------------------- */
 "pcHDTV HD-2000 TV",
 4,
 1,
 0,
 2,
 0,
 0,
 {2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 PLL_NONE,
 TUNER_PHILIPS_ATSC,
 0,
 0,
 0,
},{
 "Twinhan DST + clones",
 0,
 0,
 0,
 0,
 0,
 0,
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 1,
 1,
 0,
 0,
 PLL_NONE,
 TUNER_ABSENT,
 0,
 0,
 0,
},{
 "Winfast VC100",
 3,
 0,
 -1, // no tuner
 1,
 0,
 0,
 {3,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0},
 0,
 0,
 1,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 PLL_28,
 TUNER_ABSENT,
 0,
 0,
 0,
},{
 "Teppro TEV-560/InterVision IV-560",
 3,
 1,
 0,
 2,
 0,
 3,
 {2,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
 {1,1,1,1,0,0},
 0,
 0,
 0,
 0,
 1,
 0,
 0,
 0,
 0,
 0,
 PLL_35,
 TUNER_PHILIPS_PAL,
 0,
 0,
 0,
}};
const unsigned int bttv_num_tvcards=(sizeof(bttv_tvcards)/sizeof(struct tvcard));
#endif // NO_EXTERN_BTTV_TVCARDS

