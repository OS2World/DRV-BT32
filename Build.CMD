/* $Id: Build.CMD,v 1.17 2004/02/23 19:42:43 smilcke Exp $ */
/* Build.CMD for BT32 driver */
'@echo off'
'mode 80,60'
setlocal

Call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
Call SysLoadFuncs;
Call CheckEnvironment

LXApiDir=Value('LXAPI32DEV',,'OS2ENVIRONMENT')
LXApiEnv=LXApiDir'\LXApiEnv.CMD'
'Call 'LXApiEnv
CopyToDisk=Value('LX_COPY_TO_DISK',,'OS2ENVIRONMENT')

/* PKZIP Options */
PKZAddParms='/Add=update /NoExtended /Max'
PKZExclParms='/Excl=*.lst /Excl=*.lrf /Excl=*.lib /Excl=*.err /Excl=*.map /Excl=*.sym /Excl=*.wat /Excl=*.bak /Excl=BIN'

/* Set subdirectory on A: if copy is enabled */
TargetDirectory='\BT32'

All=0
Clean=0
Release=0

makeopts='-W NOOS2LXAPI=1'
'@SET NOOS2LXAPI=1'

Parse Arg Arguments
Do i=1 To Words(Arguments)
 A=Translate(Word(Arguments,i))
 If A='ALL' Then Clean=1
 If A='CLEAN' Then Clean=1
 If A='DEBUG' Then Release=0
 If A='RELEASE' Then Release=1
End

If All=1 Then '@call Build.cmd clean'
If All=1 Then makeopts=makeopts' all'
If Clean=1 Then makeopts=makeopts' clean'
If Release=0 Then
Do
 makeopts=makeopts' DEBUG=1'
 '@SET DEBUG=1'
End

/* Check, if previous build was release or not */
If Clean=0 Then
Do
 EA='LastBuild'
 rc=SysGetEA(Directory(),'LastBuild','WasRelease')
 If WasRelease='Debug' Then WasRelease=0
 If WasRelease='Release' Then WasRelease=1
 If (WasRelease<>Release) Then DoClean=1
 If DoClean=1 Then
 Do
  '@call Build Clean'
  '@call 'LXApiEnv
  If Release=1 Then
  Do
   rc=SysPutEA(Directory(),'LastBuild','Release')
  End
  Else
  Do
   rc=SysPutEA(Directory(),'LastBuild','Debug')
  End
 End
End

/* Get time stamp of old file */
'@copy .\tools\buildlvl.cmd .\tools\buildlvl.old > nul'
Call SysFileTree 'BIN\\*.SYS','ofi','F'
'@call 'LXApiDir'\Tools\BldMgr.cmd /File:.\Tools\buildlvl.cmd /Text:BUILD_LEVEL= /Action:INCR /Quiet'
'@call .\Tools\buildlvl.cmd'
'@make 'makeopts
If Clean=1 Then rc=1
Else If rc<>0 Then
Do
 tm=rc
 '@copy .\tools\buildlvl.old .\tools\buildlvl.cmd > nul'
 rc=tm
End
Else
Do
 IsNew=0
 Call SysFileTree 'BIN\\*.SYS','nfi','F'
 Do i=1 To nfi.0
  If nfi.i<>ofi.i Then IsNew=1
 End
 If IsNew=0 Then
 Do
  tm=rc
  '@copy .\tools\buildlvl.old .\tools\buildlvl.cmd > nul'
  rc=tm
 End
End
tm=rc
'@if exist .\tools\buildlvl.old del .\tools\buildlvl.old'
rc=tm
If Clean=1 Then CopyToDisk=0
If CopyToDisk=1 Then
Do
 If rc=0 Then
 Do
  rc=SysDriveInfo('A:')
  If (rc<>'') Then
  Do
   '@md a:\BIN 2> nul'
   '@md a:'TargetDirectory' 2> nul'
   DestFiles='bt32.sys'
   DestFiles=''
   IgnoreDirs='Linux Dev16'
   Do i=1 To Words(DestFiles)
    rc=CheckCopy(Word(DestFiles,i),IgnoreDirs)
   End
   ZipDirs='BIN BTTV Dev32 I2C Inc Include Lib Lib32 Linux PCI Makes Tools'
   Do i=1 To Words(ZipDirs)
    rc=CheckZip(Word(ZipDirs,i))
   End
   '@call pkzip 'PKZAddParms' a:'TargetDirectory'\root.zip *.*'
  End
 End
End
'@endlocal'
Exit

CheckZip: Procedure Expose TargetDirectory PKZAddParms PKZExclParms
 rc=0
 DoZip=0
 SubDir=Arg(1)
 rc=SysFileTree(SubDir'\*.*',s,'FT')
 rc=SysFileTree('A:'TargetDirectory'\'SubDir'.ZIP',d,'FT')
 If d.0>0 Then
 Do
  Do i=1 To s.0
   If Left(s.i,14)>Left(d.1,14) Then
   Do
    DoZip=1
    Leave
   End
  End
 End
 Else
 Do
  DoZip=1
 End
 If DoZip=1 Then
 Do
  '@call pkzip 'PKZAddParms' /Dir 'PKZExclParms' a:'TargetDirectory'\'SubDir'.ZIP 'SubDir'\*.*'
 End
Return rc

CheckCopy: Procedure
 rc=0
 di=Directory()'\'
 SubDir=Directory()
 DestPath='A:'
 filespec=Arg(1)
 ignoreDirs=Arg(2)
 rc=SysFileTree(filespec,sFiles,'FSO')
 /* Remove EA's from all files */
 Do i=1 To sFiles.0
  /*
  '@eautil 'sFiles.i' 'sFiles.i'.EA /s /r > nul'
  '@if exist 'sFiles.i'.EA del 'sFiles.i'.EA > nul'
  */
  sF=sFiles.i
  sF=DelStr(sF,Pos(di,sF),Length(di))
  dF=sF
  doCopy=1
  Do j=1 To Words(ignoreDirs)
   If Word(ignoreDirs,j)=Left(sF,Length(Word(ignoreDirs,j))) Then
   Do
    doCopy=0
    Leave
   End
  End
  If doCopy=1 Then
  Do
   rc=DoCheckCopy(sF,DestPath'\'dF)
  End
 End
Return rc

DoCheckCopy: Procedure
 rc=0
 sFile=Arg(1)
 dFile=Arg(2)
 rc=CreatePath(FileSpec('DRIVE',dFile)''FileSpec('PATH',dFile))
 rc=SysFileTree(sFile,s,'FT')
 rc=SysFileTree(dFile,d,'FT')
 doCopy=0
 If s.0>0 Then
 Do
  If d.0>0 Then
  Do
   If (Left(s.1,14)<>Left(d.1,14)) Then DoCopy='Update: '
  End
  Else doCopy='Add:    '
  FNN=FileSpec('NAME',dFile)
  FNE=''
  If Pos('.',FNN)>0 Then
  Do
   FNE=Right(FNN,Length(FNN)-Pos('.',FNN))
   FNN=Left(FNN,Length(FNN)-Length(FNE)-1)
  End
  If Length(FNN)>8 Then doCopy='TooLong:'
  If Length(FNE)>8 Then doCopy='TooLong:'
  If doCopy<>0 Then
  Do
   If doCopy<>'TooLong:' Then
   Do
    Say doCopy' 'sFile' To 'dFile
    '@copy 'sFile' 'dFile' > nul'
   End
/*
   If doCopy='TooLong:' Then
   Do
    Say doCopy' 'sFile
   End
*/
  End
 End
Return rc

CreatePath: Procedure
 sDir=Arg(1)
 sPath=FileSpec('PATH', sDir)
 If (Length(sPath) > 0 & sPath <> '\') Then
 Do
  rc=CreatePath(FileSpec('Drive',sDir) || subStr(sPath,1,Length(sPath)-1))
 End
 rc=SysMkDir(sDir)
Return 0

CheckEnvironment: Procedure
 rc1=CheckEnvPath('DDK','BASE\H\RMCALLS.H')
 rc2=CheckEnvPath('WATCOM','BINP\WPP386.EXE')
 rc3=CheckEnvPath('LXAPI32DEV','Tools\BldMgr.cmd')
 rc4=CheckEnvPath('GCC','include\a_out_stab.h')
 rc5=CheckEnvPath('EMXUTILS','make.exe')
 If rc1<1 | rc2<1 | rc3<1 | rc4<1 | rc5<1 Then Exit
Return 0

CheckEnvPath: Procedure
 E=Arg(1)
 F=Arg(2)
 P=Value(E,,'OS2ENVIRONMENT')
 CF=P'\'F
 rc=SysFileTree(CF,FF,'F')
 If FF.0<1 Then
 Do
  Say 'Environment variable 'E' not defined or wrong'
 End
Return FF.0
