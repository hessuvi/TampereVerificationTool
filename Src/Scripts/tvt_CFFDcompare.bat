@echo off
rem tvt.CFFDcompare Windows batch file.
if not "%2"=="" goto run
echo Usage: tvt_CFFDcompare lsts_A lsts_B
goto end

:run
tvt_taureduction -f %1 %temp%\$$A_tvt.tmp
tvt_lsts2ag -f %temp%\$$A_tvt.tmp %temp%\$$B_tvt.tmp
tvt_detag -f %temp%\$$B_tvt.tmp %temp%\$$C_tvt.tmp

tvt_taureduction -f %2 %temp%\$$A_tvt.tmp
tvt_lsts2ag -f %temp%\$$A_tvt.tmp %temp%\$$B_tvt.tmp
tvt_detag -f %temp%\$$B_tvt.tmp %temp%\$$A_tvt.tmp

tvt_SBcompare -q %temp%\$$C_tvt.tmp %temp%\$$A_tvt.tmp

del %temp%\$$A_tvt.tmp
del %temp%\$$B_tvt.tmp
del %temp%\$$C_tvt.tmp

if errorlevel 1 goto end
echo The LSTSs are CFFD-equivalent.
:end