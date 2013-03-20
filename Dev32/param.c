/* $Id: param.c,v 1.18 2004/01/29 20:40:39 smilcke Exp $ */

/*
 * param.c
 * Autor:               Stefan Milcke
 * Erstellt am:         10.12.2001
 * Letzte Aenderung am: 16.04.2003
 *
*/
#ifdef __cplusplus
extern "C" {
#endif

#define INCL_NOPMAPI
#define INCL_DOSINFOSEG
#include <os2.h>

#include <devhlp32.h>
#include <devtype.h>
#include <devrp.h>
#include <ldefos2.h>


#include <linux/string.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <lxapilib.h>

#define MAX_PARM_LENGTH 256

int verbose=0;
int verbose_modulelist=0;
int nocoremodule=0;
int nomodule=0;
extern int allow_irq;

static char PARM_LENGTH_ERROR[]="BT32: Parameter to long: ";
static char PARM_UNKNOWN[]="BT32: Unknown parameter: ";
static char PARM_MALLOC_ERROR[]="BT32: Memory allocation error\r\n";
static char CRLF[]="\r\n";

char drvhomepath[300];

char *parmList[]=
{
 "/3",
 "/VM",
 "/V",
 "/NOCOREMODULE",
 "/NOMODULE",
 "/MODPARM:",
 "/LOADMODULE:",
 "/NOIRQ",
};

int numParms=sizeof(parmList)/sizeof(char *);

//-------------------------------- load_module ---------------------------------
int load_module(char *moduleName)
{
 int rc=0;
 if(moduleName)
  rc=0;
 return rc;
}

extern "C" void WriteString(const char __far *str,int length);

//---------------------------------- parseArg ----------------------------------
int parseArg(char* arg)
{
 int i;
 for(i=0;i<numParms;i++)
 {
  if(!strncmp(arg,parmList[i],strlen(parmList[i])))
  {
   switch(i)
   {
    case 0:
     Int3();
     break;
    case 1:
     verbose_modulelist=1;
    case 2:
     verbose=1;
     break;
    case 3:
     nocoremodule=1;
    case 4:
     nomodule=1;
     break;
    case 5:
     {
      char errMsg[1024];
      errMsg[0]=(char)0;
      OS2_set_module_parm(&(arg[strlen(parmList[i])]),errMsg);
      if(strlen(errMsg))
       WriteString(errMsg,strlen(errMsg));
     }
     break;
    case 6:
     load_module(&(arg[strlen(parmList[i])]));
     break;
    case 7:
     allow_irq=0;
     break;
   }
   break;
  }
 }
 if(i>=numParms)
 {
  WriteString(PARM_UNKNOWN,strlen(PARM_UNKNOWN));
  WriteString(arg,strlen(arg));
  WriteString(CRLF,strlen(CRLF));
  return 1;
 }
 return 0;
}

//--------------------------------- parseArgs ----------------------------------
void parseArgs(RPInit __far *rp)
{
 char FAR48 *args;
 char *carg;
 char *p;
 char *sl=NULL;
 int i;
 carg=(char*)malloc(MAX_PARM_LENGTH);
 if(carg)
 {
  args=MAKE_FARPTR32(rp->In.Args);
  // Remove leading blanks
  while(*args && *args==' ')
   args++;
  // skip device=... statement while storing the path into drvhomepath
  p=drvhomepath;
  while(*args && *args!=' ')
  {
   *p=*args++;
   if(*p=='\\')
    sl=p;
   p++;
  }
  if(sl)
   *++sl=(char)0;
  while(*args)
  {
   p=carg;
   while(*args && *args==' ') args++;
   i=0;
   while(*args && *args!=' ' && i<MAX_PARM_LENGTH-1)
   {
    *p++=*args++;
    i++;
   }
   if(i>=MAX_PARM_LENGTH-1)
   {
    WriteString(PARM_LENGTH_ERROR,strlen(PARM_LENGTH_ERROR));
    WriteString(carg,i);
    WriteString(CRLF,strlen(CRLF));
   }
   else
   {
    *p=(char)0;
    if(strlen(carg))
     if(parseArg(carg))
      break;
   }
  }
  free(carg);
 }
 else
  WriteString(PARM_MALLOC_ERROR,strlen(PARM_MALLOC_ERROR));
}

#ifdef __cplusplus
}
#endif
