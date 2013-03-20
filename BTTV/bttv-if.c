/* $Id: bttv-if.c,v 1.12 2004/01/29 09:10:13 smilcke Exp $ */

/*
 * bttv-if.c
 * Autor:               Stefan Milcke
 * Erstellt am:         09.11.2001
 * Letzte Aenderung am: 29.01.2004
 *
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/i2c-id.h>
#include <linux/i2c-compat.h>

#include <asm/io.h>

#include "bttvp.h"
#include <media/tuner.h>

extern int bttv_num;

/* ----------------------------------------------------------------------- */
/* Exported functions - for other modules which want to access the         */
/*                      gpio ports (IR for example)                        */
/*                      see bttv.h for comments                            */

//----------------------------- bttv_get_cardinfo ------------------------------
int bttv_get_cardinfo(unsigned int card, int *type, unsigned *cardid)
{
 if (card >= bttv_num)
 {
  return -1;
 }
 *type   = bttvs[card].type;
 *cardid = bttvs[card].cardid;
 return 0;
}

//------------------------------ bttv_get_pcidev -------------------------------
struct pci_dev* bttv_get_pcidev(unsigned int card)
{
 if (card >= bttv_num)
  return (struct pci_dev*)NULL;
 return bttvs[card].dev;
}

//-------------------------------- bttv_get_id ---------------------------------
int bttv_get_id(unsigned int card)
{
 CPK(printk("bttv_get_id is obsolete, use bttv_get_cardinfo instead\n"));
 if (card >= bttv_num)
 {
  return -1;
 }
 return bttvs[card].type;
}

//------------------------------ bttv_gpio_enable ------------------------------
int bttv_gpio_enable(unsigned int card, unsigned long mask, unsigned long data)
{
 struct bttv *btv;
 if (card >= bttv_num)
 {
  return -EINVAL;
 }
 btv = &bttvs[card];
 btaor(data, ~mask, BT848_GPIO_OUT_EN);
 if (bttv_gpio)
  bttv_gpio_tracking(btv,"extern enable");
 return 0;
}

//------------------------------- bttv_read_gpio -------------------------------
int bttv_read_gpio(unsigned int card, unsigned long *data)
{
 struct bttv *btv;
 if (card >= bttv_num)
 {
  return -EINVAL;
 }
 btv = &bttvs[card];
 if(btv->shutdown)
 {
  return -ENODEV;
 }

/* prior setting BT848_GPIO_REG_INP is (probably) not needed
   because we set direct input on init */
 *data = btread(BT848_GPIO_DATA);
 return 0;
}

//------------------------------ bttv_write_gpio -------------------------------
int bttv_write_gpio(unsigned int card, unsigned long mask, unsigned long data)
{
 struct bttv *btv;
 if (card >= bttv_num)
 {
  return -EINVAL;
 }
 btv = &bttvs[card];

/* prior setting BT848_GPIO_REG_INP is (probably) not needed
   because direct input is set on init */
 btaor(data & mask, ~mask, BT848_GPIO_DATA);
 if (bttv_gpio)
  bttv_gpio_tracking(btv,"extern write");
 return 0;
}

//---------------------------- bttv_get_gpio_queue -----------------------------
wait_queue_head_t* bttv_get_gpio_queue(unsigned int card)
{
 struct bttv *btv;
 if (card >= bttv_num)
 {
  return (wait_queue_head_t*)NULL;
 }
 btv = &bttvs[card];
 if (bttvs[card].shutdown)
 {
  return (wait_queue_head_t*)NULL;
 }
 return &btv->gpioq;
}

/* ----------------------------------------------------------------------- */
/* I2C functions                                                           */
//------------------------------ bttv_bit_setscl -------------------------------
void bttv_bit_setscl(void *data, int state)
{
 struct bttv *btv = (struct bttv*)data;
 int tmp;
 if (state)
  btv->i2c_state |= 0x02;
 else
  btv->i2c_state &= ~0x02;
 btwrite(btv->i2c_state, BT848_I2C);
 tmp=btread(BT848_I2C);
}

//------------------------------ bttv_bit_setsda -------------------------------
void bttv_bit_setsda(void *data, int state)
{
 struct bttv *btv = (struct bttv*)data;
 int tmp;
 if (state)
  btv->i2c_state |= 0x01;
 else
  btv->i2c_state &= ~0x01;
 btwrite(btv->i2c_state, BT848_I2C);
 tmp=btread(BT848_I2C);
}

//------------------------------ bttv_bit_getscl -------------------------------
static int bttv_bit_getscl(void *data)
{
 struct bttv *btv = (struct bttv*)data;
 int state;
 state = btread(BT848_I2C) & 0x02 ? 1 : 0;
 return state;
}

//------------------------------ bttv_bit_getsda -------------------------------
static int bttv_bit_getsda(void *data)
{
 struct bttv *btv = (struct bttv*)data;
 int state;
 state = btread(BT848_I2C) & 0x01;
 return state;
}

//-------------------------------- bttv_inc_use --------------------------------
static void bttv_inc_use(struct i2c_adapter *adap)
{
 MOD_INC_USE_COUNT;
}

//-------------------------------- bttv_dec_use --------------------------------
static void bttv_dec_use(struct i2c_adapter *adap)
{
 MOD_DEC_USE_COUNT;
}

//------------------------------- attach_inform --------------------------------
static int attach_inform(struct i2c_client *client)
{
 struct bttv *btv = (struct bttv*)i2c_get_adapdata(client->adapter);
 if (btv->tuner_type != UNSET)
  bttv_call_i2c_clients(btv,TUNER_SET_TYPE,&btv->tuner_type);
 if (btv->pinnacle_id != UNSET)
  bttv_call_i2c_clients(btv,AUDC_CONFIG_PINNACLE,
                        &btv->pinnacle_id);
 if (bttv_debug)
  CPK(printk("bttv%d: i2c attach [client=%s]\n",
             btv->nr, i2c_clientname(client)));
 return 0;
}

//--------------------------- bttv_call_i2c_clients ----------------------------
void bttv_call_i2c_clients(struct bttv *btv, unsigned int cmd, void *arg)
{
 if (0 != btv->i2c_rc)
  return;
 i2c_clients_command(&btv->i2c_adap, cmd, arg);
}

//------------------------------- bttv_i2c_call --------------------------------
void bttv_i2c_call(unsigned int card, unsigned int cmd, void *arg)
{
 if (card >= bttv_num)
  return;
 bttv_call_i2c_clients(&bttvs[card], cmd, arg);
}

static struct i2c_algo_bit_data bttv_i2c_algo_template=
{
 NULL,
 bttv_bit_setsda,
 bttv_bit_setscl,
 bttv_bit_getsda,
 bttv_bit_getscl,
 16,
 10,
 200,
};

static struct i2c_adapter bttv_i2c_adap_template=
{ // place I2C_ADAP_CLASS_TV_ANALOG somewhere when I2C is complete at 2.4.24 (SM)
 "bt848",
 I2C_HW_B_BT848,
 0,
 NULL,
 bttv_inc_use,
 bttv_dec_use,
 attach_inform,
 0,
};

static struct i2c_client bttv_i2c_client_template=
{
 "bttv internal",
 -1,
};

//-------------------------------- bttv_I2CRead --------------------------------
int bttv_I2CRead(struct bttv *btv,unsigned char addr,char *probe_for)
{
 unsigned char buffer = 0;
 if (0 != btv->i2c_rc)
  return -1;
 if (bttv_verbose && NULL != probe_for)
  CPK(printk(KERN_INFO "bttv%d: i2c: checking for %s @ 0x%02x... ",
             btv->nr,probe_for,addr));
 btv->i2c_client.addr = addr >> 1;
 if (1 != i2c_master_recv(&btv->i2c_client, (char*)&buffer, 1))
 {
  if (NULL != probe_for)
  {
   if (bttv_verbose)
    CPK(printk("not found\n"));
  }
  else
   CPK(printk(KERN_WARNING "bttv%d: i2c read 0x%x: error\n",
                btv->nr,addr));
  return -1;
 }
 if (bttv_verbose && NULL != probe_for)
  CPK(printk("found\n"));
 return buffer;
}

//------------------------------- bttv_I2CWrite --------------------------------
int bttv_I2CWrite(struct bttv *btv, unsigned char addr, unsigned char b1,
                    unsigned char b2, int both)
{
 unsigned char buffer[2];
 int bytes = both ? 2 : 1;
 if (0 != btv->i2c_rc)
  return -1;
 btv->i2c_client.addr = addr >> 1;
 buffer[0] = b1;
 buffer[1] = b2;
 if (bytes != i2c_master_send(&btv->i2c_client, (char*)buffer, bytes))
  return -1;
 return 0;
}

/* read EEPROM content */
//-------------------------------- bttv_readee ---------------------------------
void __devinit bttv_readee(struct bttv *btv, unsigned char *eedata, int addr)
{
 int i;
 if (bttv_I2CWrite(btv, addr, 0, -1, 0)<0)
 {
  CPK(printk(KERN_WARNING "bttv: readee error\n"));
  return;
 }
 btv->i2c_client.addr = addr >> 1;
 for (i=0; i<256; i+=16)
 {
  if (16 != i2c_master_recv(&btv->i2c_client,(char*)(eedata+i),16))
  {
   CPK(printk(KERN_WARNING "bttv: readee error\n"));
   break;
  }
 }
}

//------------------------------- init_bttv_i2c --------------------------------
int __devinit init_bttv_i2c(struct bttv *btv)
{
 memcpy(&btv->i2c_adap, &bttv_i2c_adap_template,
        sizeof(struct i2c_adapter));
 memcpy(&btv->i2c_algo, &bttv_i2c_algo_template,
        sizeof(struct i2c_algo_bit_data));
 memcpy(&btv->i2c_client, &bttv_i2c_client_template,
        sizeof(struct i2c_client));
 sprintf(btv->i2c_adap.name, "bt848 #%d", btv->nr);
 btv->i2c_algo.data = btv;
 i2c_set_adapdata(&btv->i2c_adap, btv);
 btv->i2c_adap.algo_data = &btv->i2c_algo;
 btv->i2c_client.adapter = &btv->i2c_adap;
 bttv_bit_setscl(btv,1);
 bttv_bit_setsda(btv,1);
 btv->i2c_rc = i2c_bit_add_bus(&btv->i2c_adap);
 return btv->i2c_rc;
}

#ifdef __cplusplus
}
#endif
