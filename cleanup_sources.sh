#! /bin/sh

# cleanup.sh
#
# This script cleans up the TVT directory from files not needed
# in the public source packet.
#
# Use and update with special caution!

TVT=`pwd`

# TVT/
rm -r Docs/
rm -r Style/
rm TVT.log
rm TVT.prj
rm .TVT.prcs_aux

cd $TVT/Src/
rm -r PA_Operators/
rm README.reduction
rm -r detach/
rm -r CFFD_Test/
rm -rf CutTest/
rm linecount.sh
rm -r SystemTests/
rm -r TauLoops/
rm testing_framework.w
rm Installation.make
rm updatecopyright.sh
rm -r iLangCompiler
rm -r scriptdump
rm -r createswitch
rm -r errortracer

cd $TVT/Src/Illustrator/GUI/
rm whatsnew.txt

cd $TVT/Src/FileFormats/
rm -r fftest/
rm -r testers/
rm -r StringMatcher/

cd $TVT/Src/FileFormats/FileFormat/Lexical/
rm testtokenstreams.cc

cd $TVT/Src/createrules/
rm -r test/

cd $TVT/Src/parallel/
rm ASMemory.hh
rm AStateSet.hh
rm -r filosofit/
rm outputlsts.btree.cc
rm -r tokenring/

cd $TVT/Src/SBcompare/
rm -r test/

cd $TVT/Src/SBreduction/
rm luokkakaavio.fig
rm test1.lsts

cd $TVT/Src/FrameWork/
rm testaccsets.cc
rm testbitvector.cc
rm testmemory.cc
rm testmemorystack.cc
rm testroaccsets.cc
rm teststateprops.cc
rm testtransitionscontainer.cc

cd $TVT/Src/extendedrules/
rm -r test/
