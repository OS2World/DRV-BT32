@REM $Id: clean.cmd,v 1.10 2004/02/22 23:03:20 smilcke Exp $

@setlocal
@call %LXAPI32DEV%\LXApiEnv.CMD
@call make clean
@endlocal
