#COMPILER=g++
#OPTIMIZE=-O3
#LD=g++

-include ../../.makefile_vars

#OPTIMIZATIONS=-g

CC=$(CXX)

CFLAGS=$(CXXFLAGS) `pkg-config --cflags gtk+-2.0`
LFLAGS=`pkg-config --libs gtk+-2.0` -lm

GLDIR=.
OBJS=main.o lts_viewer.o gtklib.o graflib.o lts_draw_reader.o

main: $(OBJS)
	$(LD) $(OBJS) -o main $(LFLAGS)

main.o: main.cc lts_viewer.hh $(GLDIR)/gtklib.hh lts_draw_reader.hh
	$(CC) $(CFLAGS) main.cc -c

lts_draw_reader.o: lts_draw_reader.cc lts_draw_reader.hh point.hh
	$(CC) $(CFLAGS) lts_draw_reader.cc -c

lts_viewer.o: lts_viewer.cc lts_viewer.hh $(GLDIR)/graflib.hh point.hh constants.hh
	$(CC) $(CFLAGS) lts_viewer.cc -c

gtklib.o: $(GLDIR)/gtklib.cc $(GLDIR)/gtklib.hh $(GLDIR)/graflib.hh
	$(CC) $(CFLAGS) $(GLDIR)/gtklib.cc -c

graflib.o: $(GLDIR)/graflib.cc $(GLDIR)/graflib.hh
	$(CC) $(CFLAGS) $(GLDIR)/graflib.cc -c

windows:
	$(MAKE) -f Makefile.windows

dep:

$(DEP_FILE):

.PHONY: dep
