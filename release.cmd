@REM $Id: release.cmd,v 1.11 2004/02/22 23:03:20 smilcke Exp $
setlocal

cd Apps\StBTV4LX
if exist *.obj del *.obj
if exist *.lib del *.lib
if exist *.dll del *.dll
nmake -a

cd ..\stbtdaem
if exist *.obj del *.obj
if exist *.lib del *.lib
if exist *.dll del *.dll
nmake -a

cd ..\stbtgui
if exist *.obj del *.obj
if exist *.lib del *.lib
if exist *.dll del *.dll
nmake -a

cd ..\StBTVSD
if exist *.obj del *.obj
if exist *.lib del *.lib
if exist *.dll del *.dll
nmake -a

cd ..\bttv
if exist *.obj del *.obj
if exist *.lib del *.lib
if exist *.dll del *.dll
nmake -a

cd ..
cd ..
call build release
endlocal