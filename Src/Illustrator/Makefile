TVT_BASE = ..

SOURCES = graph.cc point.cc line.cc linealg.cc random.cc \
        lts.inst.cc \
        layout.inst.cc \
        initial.cc \
        level.inst.cc \
        nonsym.cc \
        localsea.inst.cc \
        sa.inst.cc \
        genetic.inst.cc \
        arrows.inst.cc \
        grid.inst.cc \
        surround.cc \
        styles.inst.cc \
        lts_draw_write.cc \
        plain_algo.inst.cc \
        graph_tmpl.inst.cc \
        stl.inst.cc

BIN =  illus

LOCAL_INC_DIRS = -I $(TVT_BASE)/ThirdPartySoftware
LOCAL_LIBS = -L $(TVT_BASE)/ThirdPartySoftware/lp_solve -llpk

include $(TVT_BASE)/BaseMakefile

#WARNINGS = -Wall -pedantic -ansi
