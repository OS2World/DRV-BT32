/* $Id: ExBttv.CMD,v 1.7 2002/05/01 01:19:41 smilcke Exp $ */

/* Extrahiert aus dem BTTV-Treiber die Strukturen
   Copyright (c) 2001-2001 by Stefan Milcke
                              Kferstraáe 45
                              28779 Bremen

   Datum der Erstellung: 19.08.2001
   Letzte Aenderung am:  19.08.2001
*/

InFile='Linux\BTTV\bttv-cards.c'
OutFile='bttvcrds.c'
Parse Arg P1 P2
If Length(P1)>0 Then InFile=P1
If Length(P2)>0 Then OutFile=P2

Call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
Call RxFuncAdd 'SysFileDelete','RexxUtil','SysFileDelete'
Call RxFuncAdd 'SysTempFileName','RexxUtil','SysTempFileName'

/* L”scht ggf. OutFile */
OFile=Stream(OutFile,"C","QUERY EXISTS")
If Length(OFile)>0 Then
 Call SysFileDelete OutFile

CD=Date('S')
CD=Right(CD,2)'.'SubStr(CD,5,2)'.'Left(CD,4)
IFile=Stream(InFile,"C","OPEN READ")
OFile=Stream(OutFile,"C","OPEN WRITE")
rc=LineOut(OutFile,'/*')
rc=LineOut(OutFile,' * 'OutFile)
rc=LineOut(OutFile,' * Autor:               ExBTTV.CMD (Stefan Milcke)')
rc=LineOut(OutFile,' * Erstellt am:         'CD)
rc=LineOut(OutFile,' * Letzte Aenderung am: 'CD)
rc=LineOut(OutFile,' * Translated from 'InFile)
rc=LineOut(OutFile,' *')
rc=LineOut(OutFile,'*/')
Do Forever
 If Lines(InFile)=0 Then Leave
 Line=LineIn(InFile)
 If Line='static struct CARD {' Then
 Do
  rc=LineOut(OutFile,'')
  rc=LineOut(OutFile,'/* --------------------------------- */')
  rc=LineOut(OutFile,'/* List of card IDs for Bt878+ cards */')
  rc=LineOut(OutFile,'')
  Do Forever
   Line=Strip(Translate(Line,' ',D2C(9)),'L',' ')
   If Line='} cards[] __devinitdata = {' Then Line='} cards[] = {'
   If Line='unsigned id;' Then Line='DWORD id;'
   If Left(Line,1)='{' Then Line=' 'Line
   rc=LineOut(OutFile,Line)
   If Line='};' Then Leave
   Line=LineIn(InFile)
  End
  rc=LineOut(OutFile,'')
 End
 Else
 Do
  If Line='struct tvcard bttv_tvcards[] = {' Then
  Do
   rc=LineOut(OutFile,'')
   rc=LineOut(OutFile,'/* ------------------------------------------------------- */')
   rc=LineOut(OutFile,'/* array with description for Bt848/Bt878 tv/grabber cards */')
   rc=LineOut(OutFile,'')
   rc=LineOut(OutFile,Line)
   Call ProcessStructure Line
  End
 End
End
rc=LineOut(OutFile,'')
rc=LineOut(OutFile,'const int bttv_num_tvcards=(sizeof(bttv_tvcards)/sizeof(struct tvcard));')
IFile=Stream(BTFile,"C","CLOSE")

Exit

/* Clears the TV-CARD variable */
ClearTC:
  tc.name=' **** UNKNOWN ****'
  tc.video_inputs='0,'
  tc.audio_inputs='0,'
  tc.tuner='0,'
  tc.svhs='0,'
  tc.gpiomask='0,'
  tc.muxsel.0=0
  tc.muxsel.1=0
  tc.muxsel.2=0
  tc.muxsel.3=0
  tc.muxsel.4=0
  tc.muxsel.5=0
  tc.muxsel.6=0
  tc.muxsel.7=0
  tc.audiomux.0=0
  tc.audiomux.1=0
  tc.audiomux.2=0
  tc.audiomux.3=0
  tc.audiomux.4=0
  tc.audiomux.5=0
  tc.gpiomask2='0,'
  tc.no_msp24xx='0,'
  tc.needs_tvaudio='0,'
  tc.pll='0,'
  tc.tuner_type='0,'
Return

/* Processes complete structure until end of file reached */
ProcessStructure: Procedure Expose InFile OutFile tc.
 Line=Arg(1)
 Call ClearTC
 rc=LineOut(OutFile,'{')
 Call ProcessLine Line
 Do Forever
  If Lines(InFile)=0 Then Leave
  Line=LineIn(InFile)
  Call ProcessLine Line
  If Line='}};' Then Leave
 End
Return

ProcessLine: Procedure Expose InFile OutFile tc.
 Line=RemoveLeadingWhitespace(Arg(1))
 If Left(Line,2)='/*' Then
 Do Forever
  rc=LineOut(OutFile,Line)
  If Pos('*/',Line)>0 Then Leave
  Line=LineIn(InFile)
  Line=RemoveLeadingWhitespace(Line)
 End
 Else
 Do
  If (Line='},{') | (Line='}};') Then
  Do
   rc=WriteWithComment(OutFile,' 'tc.name,'// name')
   rc=WriteWithComment(OutFile,' 'tc.video_inputs,'// video_inputs')
   rc=WriteWithComment(OutFile,' 'tc.audio_inputs,'// audio_inputs')
   rc=WriteWithComment(OutFile,' 'tc.tuner,'// tuner')
   rc=WriteWithComment(OutFile,' 'tc.svhs,'// svhs')
   rc=WriteWithComment(OutFile,' 'tc.gpiomask,'// gpiomask')
   rc=WriteWithComment(OutFile,' {'tc.muxsel.0','tc.muxsel.1','tc.muxsel.2','tc.muxsel.3','tc.muxsel.4','tc.muxsel.5','tc.muxsel.6','tc.muxsel.7'},','// muxsel')
   rc=WriteWithComment(OutFile,' {'tc.audiomux.0','tc.audiomux.1','tc.audiomux.2','tc.audiomux.3','tc.audiomux.4','tc.audiomux.5'},','// audiomux')
   rc=WriteWithComment(OutFile,' 'tc.gpiomask2,'// gpiomask2')
   rc=WriteWithComment(OutFile,' 'tc.no_msp24xx,'// no_msp24xx')
   rc=WriteWithComment(OutFile,' 'tc.needs_tvaudio,'// needs_tvaudio')
   rc=WriteWithComment(OutFile,' 'tc.pll,'// pll')
   rc=WriteWithComment(OutFile,' 'tc.tuner_type,'// tuner_type')
   rc=LineOut(OutFile,Line)
   Call ClearTC
  End
  Else
  Do
   If Left(Line,4)='name' Then
   Do
    tc.name=FormatItem(Line,5)
   End
   If Left(Line,12)='video_inputs' Then
   Do
    tc.video_inputs=FormatItem(Line,13)
   End
   If Left(Line,12)='audio_inputs' Then
   Do
    tc.audio_inputs=FormatItem(Line,13)
   End
   If Left(Line,6)='tuner:' Then
   Do
    tc.tuner=FormatItem(Line,6)
   End
   If Left(Line,4)='svhs' Then
   Do
    tc.svhs=FormatItem(Line,5)
   End
   If Left(Line,8)='gpiomask' Then
   Do
    tc.gpiomask=FormatItem(Line,9)
   End
   If Left(Line,9)='gpiomask2' Then
   Do
    tc.gpiomask2=FormatItem(Line,10)
   End
   If Left(Line,10)='no_msp24xx' Then
   Do
    tc.no_msp24xx=FormatItem(Line,11)
   End
   If Left(Line,13)='needs_tvaudio' Then
   Do
    tc.needs_tvaudio=FormatItem(Line,14)
   End
   If Left(Line,3)='pll' Then
   Do
    tc.pll=FormatItem(Line,4)
   End
   If Left(Line,10)='tuner_type' Then
   Do
    tc.tuner_type=FormatItem(Line,11)
   End
   If Left(Line,6)='muxsel' Then
   Do
    t=Strip(Strip(Right(Line,Length(Line)-7),'L'),'T',',')
    t=GetInnerContents(t)
    Do i=0 to Words(t)-1
     tc.muxsel.i=Word(t,i+1)
    End
   End
   If Left(Line,8)='audiomux' Then
   Do
    t=Strip(Strip(Right(Line,Length(Line)-9),'L'),'T',',')
    t=GetInnerContents(t)
    Do i=0 to Words(t)-1
     tc.audiomux.i=Word(t,i+1)
    End
   End
  End
 End
Return

GetInnerContents: Procedure
 Line=Arg(1)
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
 Line=RemoveLeadingWhiteSpace(Line)
Return Line

WriteWithComment: Procedure
 OutFile=Arg(1)
 Line=Arg(2)
 Comment=Arg(3)
 Do While Length(Line)<45
  Line=Line' '
 End
 rc=LineOut(OutFile,Line''Comment)
Return rc
 