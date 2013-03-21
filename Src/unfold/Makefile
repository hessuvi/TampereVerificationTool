TVT_BASE = ..

SOURCES = \
	folded/fdState.cc \
	folded/fdTransition.cc \
\
	unfolded/ufSProps.cc \
	unfolded/ufWriter.cc \
\
	compiler/syntax/TypeStructure.cc \
	compiler/syntax/ufStateProps.cc \
	compiler/syntax/Variables.cc \
	compiler/syntax/AlphabetDef.cc \
\
	compiler/ps.cc \
	compiler/oBC.cc \
	compiler/ExpressionParser.cc \
	compiler/Compiler.cc \
\
	uf_types.cc \
	VarTable.cc \
	Alphabet.cc \
	FoldedLsts.cc \
	UnfoldedLsts.cc \
\
	eval/iBC.cc \
	eval/ExpEvaluator.cc \
\
	main.cc 


BIN = unfold

include $(TVT_BASE)/BaseMakefile

t:
	./unfold test/basic.proc test/main.out -f --write-trs-asap=test/foo.out

t2:
	./unfold test/1_1.sproc test/main.out -f --write-trs-asap=test/foo.out

t3:
	./unfold test/5_5.proc_no_sp test/main.out -f --write-trs-asap=test/foo.out

create:
	./unfold test/pablo/$(PA).proc test/pablo/$(PA).new -f --write-trs-asap=test/pablo/$(PA)trs.new


comp:
	tvt.lsts2ag test/pablo/$(PA).new test/pablo/$(PA).ag.new -f
	tvt.detag test/pablo/$(PA).ag.new test/pablo/$(PA).dag.new -f

	tvt.lsts2ag test/pablo/$(PA).old test/pablo/$(PA).ag.old -f
	tvt.detag test/pablo/$(PA).ag.old test/pablo/$(PA).dag.old -f

	tvt.SBcompare test/pablo/$(PA).dag.new test/pablo/$(PA).dag.old


#	./unfold test/exp.proc test/main.out -f --write-trs-asap=test/foo.out

tidy:
	rm -f core *~ compiler/*~ compiler/syntax/*~ eval/*~ unfolded/*~

backup: tidy clean
	rm -f compiler/*.o compiler/syntax/*.o eval/*.o unfolded/*.o
	cp -r ../unfold /home/timoe/unfold7

g:
	gdb ./unfold
