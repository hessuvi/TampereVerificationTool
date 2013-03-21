@echo off
rem tvt.CFFDnormalize Windows batch file.
if not "%1"=="" goto run
echo A shell script which normalizes an LSTS with respect to CFFD-equivalence. 
echo Runs the following commands on the LSTS:
echo tvt_SBreduction, tvt_lsts2ag, tvt_detag, tvt_SBreduction,
echo tvt_ag2lsts, tvt_SBreduction

echo Usage:
echo tvt_CFFDnormalize inputlsts [outputlsts]
goto end

:run

tvt_SBreduction -f %1 %temp%\$$A_tvt.tmp
tvt_lsts2ag -f %temp%\$$A_tvt.tmp %temp%\$$B_tvt.tmp
tvt_detag -f %temp%\$$B_tvt.tmp %temp%\$$A_tvt.tmp
tvt_SBreduction -f %temp%\$$A_tvt.tmp %temp%\$$B_tvt.tmp
tvt_ag2lsts -f %temp%\$$B_tvt.tmp %temp%\$$A_tvt.tmp
tvt_SBreduction -f %temp%\$$A_tvt.tmp %2

del %temp%\$$A_tvt.tmp
del %temp%\$$B_tvt.tmp

:end