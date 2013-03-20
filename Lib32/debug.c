/* $Id: debug.c,v 1.8 2003/03/03 23:52:26 smilcke Exp $ */

/*
 * debug.c
 * Autor:               Stefan Milcke
 * Erstellt am:         05.11.2001
 * Letzte Aenderung am: 04.03.2003
 *
*/

#ifdef __cplusplus
extern "C" {
#endif

#define INCL_NOPMAPI
#define INCL_DOSERRORS           // for ERROR_INVALID_FUNCTION
#include <os2.h>


#define CR 0x0d
#define LF 0x0a


#define LEADING_ZEROES          0x8000
#define SIGNIFICANT_FIELD       0x0007

BOOL fLineTerminate=TRUE;

char hextab[]="0123456789ABCDEF";

                                        //-------------------- DecLongToASCII -
char *DecLongToASCII(char *StrPtr, ULONG lDecVal,USHORT Option,USHORT nlz)
{
 BOOL  fNonZero=FALSE;
 BOOL  fDoZero;
 ULONG Digit;
 ULONG Power=1000000000;                      // 1 billion
 ULONG z=10;
 while (Power)
 {
  Digit=0;                                                                        // Digit=lDecVal/Power
  while (lDecVal >=Power)                   // replaced with while loop
  {
   Digit++;
   lDecVal-=Power;
  }
  fDoZero=FALSE;
  if (Digit)
   fNonZero=TRUE;
  if(Digit || fNonZero || ((Power==1) && (fNonZero==FALSE)))
   fDoZero=TRUE;
  if(fDoZero || ((Option & LEADING_ZEROES) && (z<=nlz || nlz==0)))
   fDoZero=TRUE;
  if(fDoZero)
  {
   *StrPtr=(char)('0'+Digit);
   StrPtr++;
  }
  if (Power==1000000000)                    // 1 billion
   Power=100000000;
  else if (Power==100000000)
   Power=10000000;
  else if (Power==10000000)
   Power=1000000;
  else if (Power==1000000)
   Power=100000;
  else if (Power==100000)
   Power=10000;
  else if (Power==10000)
   Power=1000;
  else if (Power==1000)
   Power=100;
  else if (Power==100)
   Power=10;
  else if (Power==10)
   Power=1;
  else
   Power=0;
  if(z)
   z--;
 }
 return (StrPtr);
}
                                        //-------------------- HexWordToASCII -
                                        //-------------------- HexLongToASCII -
char  *HexLongToASCII(char  *StrPtr, ULONG wHexVal, USHORT Option,USHORT nlz)
{
 BOOL  fNonZero=FALSE;
 ULONG Digit;
 ULONG Power=0xF0000000;
 ULONG ShiftVal=28;
 ULONG z=8;
 while (Power)
 {
  Digit=(wHexVal & Power)>>ShiftVal;
  if (Digit)
   fNonZero=TRUE;
  if(Digit || fNonZero || ((Power==0x0F) && (fNonZero==FALSE)))
   *StrPtr++=hextab[Digit];
  else if(((Option & LEADING_ZEROES) && (z<=nlz || nlz==0)))
   *StrPtr++=hextab[0];
  if (Power==0xF0000000)                  // 1 billion
   Power=0xF000000;
  else if (Power==0xF000000)
   Power=0xF00000;
  else if (Power==0xF00000)
   Power=0xF0000;
  else if (Power==0xF0000)
   Power=0xF000;
  else if (Power==0xF000)
   Power=0xF00;
  else if (Power==0xF00)
   Power=0xF0;
  else if (Power==0xF0)
   Power=0xF;
  else
   Power=0;
  ShiftVal-=4;
  if(z)
   z--;
 } // end while
 return (StrPtr);
}

#ifdef  DEBUG
char BuildString[1024];
void StringOut(char *DbgStr);
#endif          // DEBUG

//------------------------- PrintfOut -
#pragma warn(off)
void _cdecl DPD(int level, char *DbgStr, ...)
#pragma warn(on)
{
  int numLeadingZeroes=level;
  char *pStr=(char *) DbgStr;
#ifdef DEBUG
   char *BuildPtr=BuildString;
   char *SubStr;
   union {
         void   *VoidPtr;
         USHORT *WordPtr;
         ULONG  *LongPtr;
         ULONG  *StringPtr;
         } Parm;
   USHORT wBuildOption;

   Parm.VoidPtr=(void *) &DbgStr;
   Parm.StringPtr++;                            // skip size of string pointer

   while (*pStr)
   {
    numLeadingZeroes=0;
    // don't overflow target
    if (BuildPtr >= (char *) &BuildString[1024-2])
     break;
    switch (*pStr)
    {
     case '%':
      wBuildOption=0;
      pStr++;
      if (*pStr=='0')
      {
       wBuildOption|=LEADING_ZEROES;
       pStr++;
       if(*pStr>='0' && *pStr<='9')
       {
        numLeadingZeroes=((int)(*pStr))-((int)'0');
        pStr++;
       }
      }
      if (*pStr=='u')                                                         // always unsigned
       pStr++;
      if (*pStr=='#')
       pStr++;
      switch(*pStr)
      {
       case 'x':
       case 'X':
       case 'p':
       case 'P':
        BuildPtr=HexLongToASCII(BuildPtr, *Parm.LongPtr++,wBuildOption,numLeadingZeroes);
        pStr++;
        continue;
       case 'd':
        BuildPtr=DecLongToASCII(BuildPtr, *Parm.LongPtr++,wBuildOption,numLeadingZeroes);
        pStr++;
        continue;
       case 's':
        SubStr=(char *)*Parm.StringPtr;
        while (0!=(*BuildPtr++ = *SubStr++));
         Parm.StringPtr++;
        BuildPtr--;                      // remove the \0
        pStr++;
        continue;
       case 'l':
        pStr++;
        switch (*pStr)
        {
         case 'x':
         case 'X':
          BuildPtr=HexLongToASCII(BuildPtr, *Parm.LongPtr++,wBuildOption,numLeadingZeroes);
          pStr++;
          continue;
         case 'd':
          BuildPtr=DecLongToASCII(BuildPtr, *Parm.LongPtr++,wBuildOption,numLeadingZeroes);
          pStr++;
          continue;
        } // end switch
        continue;                        // dunno what he wants
       case 0:
        continue;
      } // end switch
      break;
     case '\\':
      pStr++;
      switch (*pStr)
      {
       case 'n':
        *BuildPtr++=LF;
        pStr++;
        continue;
       case 'r':
        *BuildPtr++=CR;
        pStr++;
        continue;
       case 0:
        continue;
      } // end switch
      break;
    } // end switch
    *BuildPtr++=*pStr++;
   } // end while

   *BuildPtr=0;                                 // cauterize the string
   StringOut((char *) BuildString);         // print to comm port
#endif                            //DEBUG
}


#pragma warn(off)
void _cdecl DPE(char *DbgStr, ...)
#pragma warn(on)
{
   char *pStr = (char *) DbgStr;
#ifdef DEBUG
   char *BuildPtr=BuildString;
   char *SubStr;
   int numLeadingZeroes;
   union {
         void   *VoidPtr;
         USHORT *WordPtr;
         ULONG  *LongPtr;
         ULONG  *StringPtr;
         } Parm;
   USHORT wBuildOption;

   Parm.VoidPtr=(void *) &DbgStr;
   Parm.StringPtr++;                            // skip size of string pointer

   while (*pStr)
      {
       numLeadingZeroes=0;
      // don't overflow target
      if (BuildPtr >= (char *) &BuildString[1024-2])
         break;

      switch (*pStr)
         {
         case '%':
            wBuildOption=0;
            pStr++;
            if (*pStr=='0')
               {
               wBuildOption|=LEADING_ZEROES;
               pStr++;
       if(*pStr>='0' && *pStr<='9')
       {
        numLeadingZeroes=((int)(*pStr))-((int)'0');
        pStr++;
       }
               }
            if (*pStr=='u')                                                         // always unsigned
               pStr++;

            switch(*pStr)
               {
               case 'x':
	       case 'X':
                  BuildPtr=HexLongToASCII(BuildPtr, *Parm.LongPtr++,wBuildOption,numLeadingZeroes);
                  pStr++;
                  continue;

               case 'd':
                  BuildPtr=DecLongToASCII(BuildPtr, *Parm.LongPtr++,wBuildOption,numLeadingZeroes);
                  pStr++;
                  continue;

               case 's':
                  SubStr=(char *)*Parm.StringPtr;
                  while (0!=(*BuildPtr++ = *SubStr++));
                  Parm.StringPtr++;
                  BuildPtr--;                      // remove the \0
                  pStr++;
                  continue;

               case 'l':
                  pStr++;
                  switch (*pStr)
                  {
                  case 'x':
                  case 'X':
                  BuildPtr=HexLongToASCII(BuildPtr, *Parm.LongPtr++,wBuildOption,numLeadingZeroes);
                  pStr++;
                  continue;

                  case 'd':
                     BuildPtr=DecLongToASCII(BuildPtr, *Parm.LongPtr++,wBuildOption,numLeadingZeroes);
                     pStr++;
                     continue;
                  } // end switch
                  continue;                        // dunno what he wants

               case 0:
                  continue;
               } // end switch
            break;

      case '\\':
         pStr++;
         switch (*pStr)
            {
            case 'n':
            *BuildPtr++=LF;
            pStr++;
            continue;

            case 'r':
            *BuildPtr++=CR;
            pStr++;
            continue;

            case 0:
            continue;
            } // end switch

         break;
         } // end switch

      *BuildPtr++=*pStr++;
      } // end while

   *BuildPtr=0;                                 // cauterize the string
   StringOut((char *) BuildString);         // print to comm port
#endif                            //DEBUG
}

#ifdef DEBUG                            //------------------------- StringOut -
void CharOut(char c);
void StringOut(char *DbgStr)
{
   while (*DbgStr)
      CharOut(*DbgStr++);

   if (fLineTerminate)
   {
      CharOut(CR);                              // append carriage return,
      CharOut(LF);                              // linefeed
   }
}
#endif

#ifdef DEBUG
//#define       MAGIC_COMM_PORT 0x3f8           // pulled from word ptr 40:0
#define         MAGIC_COMM_PORT 0x2f8           // pulled from word ptr 40:0


#define UART_DATA               0x00            // UART Data port
#define UART_INT_ENAB           0x01            // UART Interrupt enable
#define UART_INT_ID             0x02            // interrupt ID
#define UART_LINE_CTRL          0x03            // line control registers
#define UART_MODEM_CTRL         0x04            // modem control register
#define UART_LINE_STAT          0x05            // line status register
#define UART_MODEM_STAT         0x06            // modem status regiser
#define UART_DIVISOR_LO         0x00            // divisor latch least sig
#define UART_DIVISOR_HI         0x01h           // divisor latch most sig

#define DELAY   nop
#endif

#ifdef DEBUG                            //--------------------------- CharOut -
void CharOut(char c)
{
        _asm    {

        mov     dx, MAGIC_COMM_PORT     // address of PS/2's first COM port
        add     dx, UART_LINE_STAT

ReadyCheck:
        in      al, dx                                                          // wait for comm port ready signal

        DELAY
        DELAY
        DELAY

        test    al, 020h
        jz      ReadyCheck

        // Send the character

        add     dx, UART_DATA - UART_LINE_STAT
        mov     al,c
        out     dx, al

        DELAY
        DELAY
        DELAY
        }
}
#endif

#ifdef __cplusplus
}
#endif