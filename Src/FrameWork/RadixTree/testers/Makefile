COMPILER=g++
COMP=$(COMPILER) -W -Wall -pedantic -ansi -O3 -mv8 -c # -I/usr/oma/timoe/Np/StringTree
SRC=/usr/oma/timoe/newTVT/Src
RUN_FILE=run_tests
LEX=$(SRC)/FileFormats/FileFormat/Lexical
FW=$(SRC)/FrameWork
FF=$(SRC)/FileFormats
PROGRAM=$(METHOD)
RES=$(METHOD).res

keycodec:
	$(COMP) KeyCoDecTester.cc -o KeyCoDecTester.o -I$(LEX) -I$(FW)
	$(COMPILER) KeyCoDecTester.o -o keycodecTester.bin -L$(SRC) -ltvt
	time ./keycodecTester.bin

clean:
	rm -f *.o *.bin *~
