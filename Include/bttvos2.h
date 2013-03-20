/* $Id: bttvos2.h,v 1.7 2002/05/01 01:19:39 smilcke Exp $ */

/*
 * bttvos2.h
 * Autor:               Stefan Milcke
 * Erstellt am:         14.11.2001
 * Letzte Aenderung am: 14.02.2002
 *
*/

#ifndef BTTVOS2_H_INCLUDED
#define BTTVOS2_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern int bttv_num;
char *OS2_bttv_get_name(int nr);
char *OS2_bttv_get_tuner_name(int nr);
int OS2_bttv_get_tuner_type(int nr);

char *OS2_tun_get_tuner_type(int nr);

void OS2_bttv_get_eeprom(unsigned char *data);

#ifdef __cplusplus
}
#endif

#endif //BTTVOS2_H_INCLUDED
