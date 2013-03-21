Tampere Verification Tool
=========================

  Introduction
  ------------

  Home page for Tampere Verification Tool (TVT) is
  http://www.cs.tut.fi/ohj/VARG/TVT.

  Documentation is not included in this repository, but can be found
  online: http://www.cs.tut.fi/ohj/VARG/TVT/#E

  Please note: The significant part of TVT is published under NOKOS
  (http://opensource.org/licenses/nokia.html) license.


  Compiling the TVT version 3.1.1
  -------------------------------

  The following auxiliary software must be present in your system:

  1) GNU make: the TVT Makefile system is built relying on GNU make
     and won't propably work with other versions of make.

  2) gtk 2.0: the graph viewer program tvt.illux requires this
     library.  An easy way to test whether gtk is installed in your
     system is to type the command:
     `pkg-config --exists gtk+-2.0 && echo YES'.

     If this command outputs `YES', gtk is present.

     Originally tvt.illux used gtk version 1.2.9. Currently it is
     modified to compile with gtk 2.24.10.

  The compilation of TVT is started by typing the command `make' in
  the root directory of TVT. After the compilation is finished, there
  should be copies of all the TVT programs in the bin
  directory. If the compilation was successful, there should be the
  following 24 programs and scripts in the bin directory:

  tvt.CFFD_normalize
  tvt.CFFDcompare
  tvt.ReduceKsnp
  tvt.SBcompare
  tvt.SBreduction
  tvt.BBreduction
  tvt.ag2lsts
  tvt.ag2tp
  tvt.compiler
  tvt.createrules
  tvt.detag
  tvt.detlsts
  tvt.extendedrules
  tvt.fftool
  tvt.illus
  tvt.illux
  tvt.illux_view
  tvt.lsts2ag
  tvt.parallel
  tvt.simulate
  tvt.taureduction
  tvt.testall
  tvt.testerrules
  tvt.unfold


  Changing compiler and linker
  ----------------------------

  The default compiler and linker command the TVT uses is g++. You may
  easily change that by creating a file named "comp_settings" under
  the directory tvt_v3.1.1/Src/ and defining variables CXX and LD in the
  file. Another useful variable is OPTIMIZATIONS. For example, the
  file "comp_settings" with the lines

   CXX = g++3.2
   LD = g++3.2
   OPTIMIZATIONS =

  makes the TVT compile without any optimizations (-O3 is the default
  flag for optimization), using gcc 3.2 as a compiler and a linker.


  Tested platforms
  ----------------

  The TVT version 3.1 has been compiled and tested on the following
  platforms:

   * GCC 3.4.2 on Sparc, Sun Solaris 8: works fine.


  Documentation of the TVT software
  ---------------------------------

  Please visit the page http://www.cs.tut.fi/ohj/VARG/TVT/index.html
  to get the newest versions of the TVT online documentation. All the
  documents available are listed in Section "TVT online documentation".


---------------

Last modifications to this document: Thu Mar 21 13:42:38 EET 2013
