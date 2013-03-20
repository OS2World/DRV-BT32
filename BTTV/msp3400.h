/* $Id: msp3400.h,v 1.9 2004/01/29 09:10:16 smilcke Exp $ */

/*
 * msp3400.h
 * Autor:               Stefan Milcke
 * Erstellt am:         19.11.2001
 * Letzte Aenderung am: 27.01.2004
 *
*/

#ifndef MSP3400_H_INCLUDED
#define MSP3400_H_INCLUDED

struct msp_dfpreg {
    int reg;
    int value;
};

#define MSP_SET_DFPREG     _IOW('m',15,struct msp_dfpreg)
#define MSP_GET_DFPREG     _IOW('m',16,struct msp_dfpreg)

#endif //MSP3400_H_INCLUDED
