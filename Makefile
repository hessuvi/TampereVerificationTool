
CP="cp"
CPFLAGS="-pu"

all:
	$(MAKE) -C Src
	mkdir -p -m 2755 bin
	$(CP) $(CPFLAGS) Src/InputLanguage/InputLanguage bin/tvt.compiler
	$(CP) $(CPFLAGS) Src/parallel/parallel bin/tvt.parallel
	$(CP) $(CPFLAGS) Src/createrules/createrules bin/tvt.createrules
	$(CP) $(CPFLAGS) Src/extendedrules/extendedrules bin/tvt.extendedrules
	$(CP) $(CPFLAGS) Src/detlsts/detag bin/tvt.detag
	$(CP) $(CPFLAGS) Src/lsts2ag/lsts2ag bin/tvt.lsts2ag
	$(CP) $(CPFLAGS) Src/ag2lsts/ag2lsts bin/tvt.ag2lsts
	$(CP) $(CPFLAGS) Src/ag2tp/ag2tp bin/tvt.ag2tp
	$(CP) $(CPFLAGS) Src/SBreduction/SBreduction bin/tvt.SBreduction
	$(CP) $(CPFLAGS) Src/TauReduction/TauReduction bin/tvt.taureduction
	$(CP) $(CPFLAGS) Src/BBreduction/BBreduction bin/tvt.BBreduction
	$(CP) $(CPFLAGS) Src/SBcompare/SBcompare bin/tvt.SBcompare
	$(CP) $(CPFLAGS) Src/ReduceKsnp/ReduceKsnp bin/tvt.ReduceKsnp
	$(CP) $(CPFLAGS) Src/simulate/simulate bin/tvt.simulate
	$(CP) $(CPFLAGS) Src/unfold/unfold bin/tvt.unfold
	$(CP) $(CPFLAGS) Src/testerrules/testerrules bin/tvt.testerrules
	$(CP) $(CPFLAGS) Src/Illustrator/illus bin/tvt.illus
	$(CP) $(CPFLAGS) Src/Illustrator/GUI/main bin/tvt.illux_view
#	strip bin/*
	$(CP) $(CPFLAGS) Src/ag2tp/testall bin/tvt.testall # Cannot be converted to Win.
	$(CP) $(CPFLAGS) Src/Illustrator/tvt.illux bin/tvt.illux
	$(CP) $(CPFLAGS) Src/Scripts/tvt.CFFD_normalize bin/
	$(CP) $(CPFLAGS) Src/Scripts/tvt.CFFDcompare bin/
	$(CP) $(CPFLAGS) Src/Scripts/tvt.detlsts bin/
#	chmod u+x bin/tvt.*


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
