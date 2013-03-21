TVT_BASE = ..

include sources

SOURCES = $(FRAMEWORK_SOURCES)

include $(TVT_BASE)/BaseMakefile

test%: test%.cc unittest.cc
	$(CXX) $(CPPFLAGS) $(CXXLIBS) $^ -o $@ $(LDFLAGS)
