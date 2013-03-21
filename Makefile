all:
	$(MAKE) -C Src
	cp Src/InputLanguage/InputLanguage bin/tvt.compiler
	cp Src/parallel/parallel bin/tvt.parallel
	cp Src/createrules/createrules bin/tvt.createrules
	cp Src/extendedrules/extendedrules bin/tvt.extendedrules
	cp Src/detlsts/detag bin/tvt.detag
	cp Src/lsts2ag/lsts2ag bin/tvt.lsts2ag
	cp Src/ag2lsts/ag2lsts bin/tvt.ag2lsts
	cp Src/ag2tp/ag2tp bin/tvt.ag2tp
	cp Src/SBreduction/SBreduction bin/tvt.SBreduction
	cp Src/TauReduction/TauReduction bin/tvt.taureduction
	cp Src/BBreduction/BBreduction bin/tvt.BBreduction
	cp Src/SBcompare/SBcompare bin/tvt.SBcompare
	cp Src/ReduceKsnp/ReduceKsnp bin/tvt.ReduceKsnp
	cp Src/simulate/simulate bin/tvt.simulate
	cp Src/unfold/unfold bin/tvt.unfold
	cp Src/testerrules/testerrules bin/tvt.testerrules
	strip bin/*
	cp Src/ag2tp/testall bin/tvt.testall # Cannot be converted to Win.
	cp Src/Illustrator/tvt.illux bin/tvt.illux
	cp Src/Scripts/tvt.CFFD_normalize bin/
	cp Src/Scripts/tvt.CFFDcompare bin/
	cp Src/Scripts/tvt.detlsts bin/
	chmod u+x bin/tvt.*

#	
#	
#	cp Src/Illustrator/illus bin/tvt.illus
#	cp Src/Illustrator/GUI/main bin/tvt.illux_view


windows:
	$(MAKE) -C Src
	mkdir bin
	move Src\InputLanguage\InputLanguage.exe bin\tvt_compiler.exe
	move Src\parallel\parallel.exe bin\tvt_parallel.exe
	move Src\createrules\createrules.exe bin\tvt_createrules.exe
	move Src\extendedrules\extendedrules.exe bin\tvt_extendedrules.exe
	move Src\detlsts\detlsts.exe bin\tvt_detlsts.exe
	move Src\lsts2ag\lsts2ag.exe bin\tvt_lsts2ag.exe
	move Src\ag2lsts\ag2lsts.exe bin\tvt_ag2lsts.exe
	move Src\ag2tp\ag2tp.exe bin\tvt_ag2tp.exe
	move Src\SBreduction\SBreduction.exe bin\tvt_SBreduction.exe
	move Src\TauReduction\TauReduction.exe bin\tvt_taureduction.exe
	move Src\BBreduction\BBreduction.exe bin\tvt_BBreduction.exe
	move Src\SBcompare\SBcompare.exe bin\tvt_SBcompare.exe
	move Src\ReduceKsnp\ReduceKsnp.exe bin\tvt_ReduceKsnp.exe
	move Src\simulate\simulate.exe bin\tvt_simulate.exe
	move Src\testerrules\testerrules bin\tvt_testerrules.exe
	copy Src\Scripts\tvt_CFFDnormalize.bat bin
	copy Src\Scripts\tvt_CFFDcompare.bat bin
	copy Src\Illustrator\tvt_illux.bat bin

#move Src\fftool\fftool.exe bin\tvt_fftool.exe
#move Src\Illustrator\illus.exe bin\tvt_illus.exe
#move Src\Illustrator\gui\main.exe bin\tvt_illux_view.exe
