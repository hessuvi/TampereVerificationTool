LSTSEDITORSOURCES=ActionNameEditor.cc DrawingTools.cc \
	LSTSEditor.cc \
	LSTSEditorMainDrawingWindow.cc \
	LSTSEditorMainDrawingWindowCalculateLayout.cc \
	LSTSEditorMainDrawingWindowFileHandling.cc \
	LSTSEditorMainDrawingWindowState.cc \
	LSTSEditorMainDrawingWindowTransitionsHolder.cc \
	ILangEditorTextWindow.cc \
	SystemTools.cc

GUISOURCES=$(LSTSEDITORSOURCES) InputLanguageSource.cc LSTSEditorSource.cc \
	MainDrawingWindow.cc MainTextWindow.cc MultipleInputOperation.cc \
	Operation.cc OperationParComp.cc OperationSBRed.cc SourceProcess.cc \
	TVTGUI.cc TVTItem.cc gui.cc icons/tvtIcons.cc

LSTSEDITORMAIN=tvt.lstseditor.cc
ILANGEDITORMAIN=tvt.ilangeditor.cc

CXX=g++
INCLUDES=-I../FileFormats -I../FrameWork
CXXFLAGS=-O3 -Wall -W -pedantic -ansi $(INCLUDES) `pkg-config --cflags gtk+-2.0`
LDLIBS=../libtvt.a `pkg-config --libs gtk+-2.0`

LINK.o=$(LINK.cpp)


gui: $(GUISOURCES:.cc=.o) ../libtvt.a

tvt.lstseditor: $(LSTSEDITORMAIN:.cc=.o) $(LSTSEDITORSOURCES:.cc=.o) \
	../libtvt.a

tvt.ilangeditor: $(ILANGEDITORMAIN:.cc=.o) $(LSTSEDITORSOURCES:.cc=.o) \
	../libtvt.a

.dep:
	g++ -MM -DMAKEDEPEND $(INCLUDES) $(GUISOURCES) $(LSTSEDITORMAIN) $(ILANGEDITORMAIN) > .dep

-include .dep
