/* $Id: blddat.c,v 1.8 2003/03/03 23:52:24 smilcke Exp $ */

/*
 * blddat.c
 * Autor:               Stefan Milcke
 * Erstellt am:         12.03.2002
 * Letzte Aenderung am: 04.03.2003
 *
*/

#ifdef __cplusplus
extern "C" {
#endif

char *buildDate=__DATE__;

//-------------------------------- getBuildDay ---------------------------------
int getBuildDay(void)
{
 int d=0;
 d=buildDate[5]-'0';
 if(buildDate[4]!=' ')
  d=d+((buildDate[4]-'0')*10);
 return d;
}

//------------------------------- getBuildMonth --------------------------------
int getBuildMonth(void)
{
 int m=0;
 switch(buildDate[0])
 {
  case 'J':
   if(buildDate[1]=='a')
    m=1;
   else if(buildDate[2]=='n')
    m=6;
   else
    m=7;
   break;
  case 'F':
   m=2;
   break;
  case 'M':
   if(buildDate[2]=='r')
    m=3;
   else
    m=5;
   break;
  case 'A':
   if(buildDate[1]=='p')
    m=4;
   else
    m=8;
   break;
  case 'S':
   m=9;
   break;
  case 'O':
   m=10;
   break;
  case 'N':
   m=11;
   break;
  case 'D':
   m=12;
   break;
 }
 return m;
}

//-------------------------------- getBuildYear --------------------------------
int getBuildYear(void)
{
 int y=0;
 y= ((buildDate[7] -'0')*1000)
   +((buildDate[8] -'0')*100 )
   +((buildDate[9] -'0')*10  )
   +((buildDate[10]-'0')     );
 return y;
}

#ifdef __cplusplus
}
#endif

