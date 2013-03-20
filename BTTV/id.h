/* $Id: id.h,v 1.9 2003/10/01 19:11:15 smilcke Exp $ */

/* FIXME: this temporarely, until these are included in linux/i2c-id.h */
#include <linux/i2c-id.h>

/* drivers */
/*
#ifndef  I2C_DRIVERID_TVMIXER
# define I2C_DRIVERID_TVMIXER I2C_DRIVERID_EXP0
#endif
#ifndef  I2C_DRIVERID_TVAUDIO
# define I2C_DRIVERID_TVAUDIO I2C_DRIVERID_EXP1
#endif
*/

/* chips */
/*
#ifndef  I2C_DRIVERID_DPL3518
# define I2C_DRIVERID_DPL3518 I2C_DRIVERID_EXP2
#endif
#ifndef  I2C_DRIVERID_TDA9873
# define I2C_DRIVERID_TDA9873 I2C_DRIVERID_EXP3
#endif
#ifndef  I2C_DRIVERID_TDA9875
# define I2C_DRIVERID_TDA9875 I2C_DRIVERID_EXP0+4
#endif
*/
#ifndef  I2C_DRIVERID_PIC16C54_PV951
# define I2C_DRIVERID_PIC16C54_PV951 I2C_DRIVERID_EXP0+5
#endif
/*
#ifndef  I2C_DRIVERID_TDA7432
# define I2C_DRIVERID_TDA7432 I2C_DRIVERID_EXP0+6
#endif
*/

/* algorithms */
#ifndef I2C_ALGO_SAA7134
# define I2C_ALGO_SAA7134 0x090000
#endif
