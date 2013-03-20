/* $Id: CnvBttv.CMD,v 1.11 2004/01/29 09:10:25 smilcke Exp $ */

/* Generates files that can be compiled via Watcom from BTTV sources
   Copyright (c) 2001-2001 by Stefan Milcke
                              KÅferstra·e 45
                              28779 Bremen

   Datum der Erstellung: 06.10.2001
   Letzte Aenderung am:  06.10.2001
*/
Call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
Call SysLoadFuncs;

Parse Arg SourceDir TargetDir
If Length(SourceDir)<1 Then SourceDir='F:\Prog\Driver\BT32\Linux\BTTV'
If Length(TargetDir)<1 Then TargetDir='F:\Prog\Driver\BT32\BTTV'

/* rc=BttvConvert(SourceDir'\bttv-driver.c',TargetDir,'bttv-driver.h') */
rc=BttvConvert(SourceDir'\bttv-cards.c',TargetDir,'bttv-cards.h')
Exit

NoComment=0

BttvConvert: Procedure Expose NoComment
 rc=0
 SFile=Arg(1)
 DFile=Arg(2)'\'Arg(3)
 If Length(Stream(DFile,'C','QUERY EXISTS'))>0 Then Call SysFileDelete DFile
 rc=Stream(SFile,'C','OPEN READ')
 rc=Stream(DFile,'C','OPEN WRITE')
 rc=WritePartFromCmd(DFile,'head.'Arg(3))
 Do Forever
  NoComment=0
  If Lines(SFile)=0 Then Leave
  Line=LineIn(SFile)
  If Line='struct tvcard bttv_tvcards[] = {' Then
  Do
   rc=LineOut(DFile,Line)
   rc=ProcessCARDStructure(Line)
  End
/*
  Else
  Do
   rc=LineOut(DFile,Line)
  End
*/
 End
 rc=WritePartFromCmd(DFile,'foot.'Arg(3))
 rc=Stream(SFile,'C','CLOSE')
 rc=Stream(DFile,'C','CLOSE')
Return rc

/* Clears the TV-CARD variable */
ClearTVCARD: Procedure Expose tc. NoComment
 tc.name=' **** UNKNOWN ****'
 tc.video_inputs='0,'
 tc.audio_inputs='0,'
 tc.tuner='0,'
 tc.svhs='0,'
 tc.digital_mode='0,';
 tc.gpiomask='0,'
 tc.gpiomask2='0,'
 tc.muxsel.0=0
 tc.muxsel.1=0
 tc.muxsel.2=0
 tc.muxsel.3=0
 tc.muxsel.4=0
 tc.muxsel.5=0
 tc.muxsel.6=0
 tc.muxsel.7=0
 tc.muxsel.8=0
 tc.muxsel.9=0
 tc.muxsel.10=0
 tc.muxsel.11=0
 tc.muxsel.12=0
 tc.muxsel.13=0
 tc.muxsel.14=0
 tc.muxsel.15=0
 tc.audiomux.0=0
 tc.audiomux.1=0
 tc.audiomux.2=0
 tc.audiomux.3=0
 tc.audiomux.4=0
 tc.audiomux.5=0
 tc.gpiomask2='0,'
 tc.no_msp24xx='0,'
 tc.no_tda9875='0,'
 tc.no_tda7432='0,'
 tc.needs_tvaudio='0,'
 tc.msp34xx_alt='0,'
 tc.no_video='0,'
 tc.has_dvb='0,'
 tc.has_remote='0,'
 tc.no_gpioirq='0,'
 tc.pll='PLL_NONE,'
 tc.tuner_type='0,'
 tc.has_radio='0,'
 tc.audio_hook='0,'
 tc.muxsel_hook='0,'
Return 1

/* Processes complete CARD structure */
ProcessCARDStructure: Procedure Expose SFile DFile NoComment
 Line=Arg(1)
 rc=ClearTVCARD()
 rc=LineOut(DFile,'{')
 rc=ProcessCARDLine(Line)
 Do Forever
  If Lines(SFile)=0 Then Leave
  Line=LineIn(SFile)
  rc=ProcessCARDLine(Line)
  If Line='}};' Then Leave
 End
Return 1

ProcessCARDLine: Procedure Expose SFile DFile tc. NoComment
 Line=RemoveLeadingWhitespace(Arg(1))
 If Left(Line,2)='/*' Then
 Do Forever
  rc=LineOut(DFile,Line)
  If Pos('*/',Line)>0 Then Leave
  Line=LineIn(SFile)
  Line=RemoveLeadingWhitespace(Line)
 End
 Else
 Do
  If (Line='},{') | (Line='}};') Then
  Do
   rc=WriteWithComment(DFile,' 'tc.name,'// name')
   rc=WriteWithComment(DFile,' 'tc.video_inputs,'// video_inputs')
   rc=WriteWithComment(DFile,' 'tc.audio_inputs,'// audio_inputs')
   rc=WriteWithComment(DFile,' 'tc.tuner,'// tuner')
   rc=WriteWithComment(DFile,' 'tc.svhs,'// svhs')
   rc=WriteWithComment(DFile,' 'tc.digital_mode,'// digital_mode')
   rc=WriteWithComment(DFile,' 'tc.gpiomask,'// gpiomask')
   rc=WriteWithComment(DFile,' {'tc.muxsel.0','tc.muxsel.1','tc.muxsel.2','tc.muxsel.3','tc.muxsel.4','tc.muxsel.5','tc.muxsel.6','tc.muxsel.7','tc.muxsel.8','tc.muxsel.9','tc.muxsel.10','tc.muxsel.11','tc.muxsel.12','tc.muxsel.13','tc.muxsel.14','tc.muxsel.15'},','// muxsel')
   rc=WriteWithComment(DFile,' {'tc.audiomux.0','tc.audiomux.1','tc.audiomux.2','tc.audiomux.3','tc.audiomux.4','tc.audiomux.5'},','// audiomux')
   rc=WriteWithComment(DFile,' 'tc.gpiomask2,'// gpiomask2')
   rc=WriteWithComment(DFile,' 'tc.no_msp24xx,'// no_msp24xx')
   rc=WriteWithComment(DFile,' 'tc.no_tda9875,'// no_tda9875')
   rc=WriteWithComment(DFile,' 'tc.no_tda7432,'// no_tda7432')
   rc=WriteWithComment(DFile,' 'tc.needs_tvaudio,'// needs_tvaudio')
   rc=WriteWithComment(DFile,' 'tc.msp34xx_alt,'// msp34xx_alt')
   rc=WriteWithComment(DFile,' 'tc.no_video,'// no_video')
   rc=WriteWithComment(DFile,' 'tc.has_dvb,'// has_dvb')
   rc=WriteWithComment(DFile,' 'tc.has_remote,'// has_remote')
   rc=WriteWithComment(DFile,' 'tc.no_gpioirq,'// no_gpioirq')
   rc=WriteWithComment(DFile,' 'tc.pll,'// pll')
   rc=WriteWithComment(DFile,' 'tc.tuner_type,'// tuner_type')
   rc=WriteWithComment(DFile,' 'tc.has_radio,'// has_radio')
   rc=WriteWithComment(DFile,' 'tc.audio_hook,'// audio_hook')
   rc=WriteWithComment(DFile,' 'tc.muxsel_hook,'// muxsel_hook')
   rc=LineOut(DFile,Line)
   NoComment=1
   Call ClearTVCARD
  End
  Else
  Do
   If ProcessStructItem(Line,'name muxsel_hook audio_hook video_inputs audio_inputs tuner_type tuner svhs gpiomask2 gpiomask no_msp24xx no_tda9875 needs_tvaudio msp34xx_alt no_video has_dvb has_remote no_gpioirq pll','tc')=0 Then
   Do
    If Left(Line,7)='.muxsel' Then
    Do
     t=Strip(Strip(Right(Line,Length(Line)-7),'L'),'T',',')
     if Left(t,1)="=" Then t=Right(t,Length(t)-1)
     t=GetInnerCARDContents(t)
     Do i=0 to Words(t)-1
      tc.muxsel.i=Word(t,i+1)
     End
    End
    Else If Left(Line,9)='.audiomux' Then
    Do
     t=Strip(Strip(Right(Line,Length(Line)-9),'L'),'T',',')
     if Left(t,1)="=" Then t=Right(t,Length(t)-1)
     t=GetInnerCARDContents(t)
     Do i=0 to Words(t)-1
      tc.audiomux.i=Word(t,i+1)
     End
    End
   End
  End
 End
Return 1

ProcessStructItem:
 rc=0
 Line=Arg(1)
 Items=Arg(2)
 Target=Arg(3)
 Do i=1 To Words(Items)
  W=Word(Items,i)
  If Left(Line,Length(W)+1)='.'W Then
  Do
   Interpret Target'.'W'=FormatItem(Line,Length(W)+1)'
   rc=1
   Leave
  End
 End
Return rc

GetInnerCARDContents: Procedure
 Line=Arg(1)
 Do While Pos('}',Line)<>0
  Line=Left(Line,Pos('}',Line)-1)
 End
 Line=RemoveLeadingWhitespace(Line)
 Line=Strip(Line,'L','{')
 Line=Strip(Line,'T','}')
 Line=RemoveLeadingWhitespace(Line)
 Line=Translate(Line,' ',',')
 Line=Translate(Line,' ',D2C(9))
Return Line

RemoveLeadingWhitespace: Procedure
 Line=Arg(1)
 Line=Strip(Translate(Line,' ',D2C(9)),'L',' ')
Return Line

FormatItem: Procedure
 Line=Arg(1)
 Le=Arg(2)
 Line=Right(Line,Length(Line)-Le)
 Do While ((Left(Line,1)=' ') | (Left(Line,1)='='))
  Line=Right(Line,Length(Line)-1)
 End
 Line=RemoveLeadingWhiteSpace(Line)
Return Line

WriteWithComment: Procedure Expose NoComment
 OutFile=Arg(1)
 Line=Arg(2)
 Comment=Arg(3)
 If NoComment<>1 Then
 Do
  Do While Length(Line)<45
   Line=Line' '
  End
 End
 If NoComment<>1 Then
 Do
  rc=LineOut(OutFile,Line''Comment)
 End
 Else
 Do
  rc=LineOut(OutFile,Line)
 End
Return rc

WritePartFromCmd: Procedure
 dFile=Arg(1)
 delim=Arg(2)
 rc=0
 Do i=1 To SourceLine()
  If SourceLine(i)='<'delim'>' Then
  Do
   rc=1
   i=i+1
   Do While (SourceLine(i)<>'</'delim'>') & i<SourceLine()
    L=SourceLine(i)
    If Pos('<%CUR_DATE%>',L)>0 Then
    Do
     L1=Left(L,Pos('<%CUR_DATE%>',L)-1)
     L2=Right(L,Length(L)-Length(L1)-Length('<%CUR_DATE%>'))
     L=L1''Translate(Date('E'),'.','/')''L2
     L=L1''Right(Date('S'),2)'.'Right(Left(Date('S'),6),2)'.'Left(Date('S'),4)''L2
    End
    rc=LineOut(dFile,L)
    i=i+1
   End
   Leave
  End
 End
Return rc


/* The following lines are copied to the generated source */
<head.bttv-cards.h>
/* $Id: CnvBttv.CMD,v 1.11 2004/01/29 09:10:25 smilcke Exp $ */

/*
 * bttv-cards.h
 * Autor:               Stefan Milcke (Automatic generation from CnvBttv.CMD)
 * Erstellt am:         <%CUR_DATE%>
 * Letzte Aenderung am: <%CUR_DATE%>
 *
*/

#include <media/tuner.h>
#include "bttv.h"
#include "bttvp.h"

#ifndef NO_EXTERN_BTTV_TVCARDS
//extern struct tvcard bttv_tvcards[];
//extern const int bttv_num_tvcards;
#else
</head.bttv-cards.h>

<foot.bttv-cards.h>
const unsigned int bttv_num_tvcards=(sizeof(bttv_tvcards)/sizeof(struct tvcard));
#endif // NO_EXTERN_BTTV_TVCARDS

</foot.bttv-cards.h>
