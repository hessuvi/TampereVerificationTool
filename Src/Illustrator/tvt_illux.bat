@echo off
rem tvt.illux Windows batch file.
rem Inputs an LSTS file and a possible style file to illus and output of
rem   illus to illux_view.
tvt_illus %1 %2 | tvt_illux_view
