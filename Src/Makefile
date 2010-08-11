# The main Makefile of TVT. GNU make is required to run this
#   file. Works both in Unix/Linux and Windows systems (for compiling TVT in
#   Windows, please take a look at the file "comp_settings.windows").

include .makefile_vars

TVT_BASE = .

SYSDEPS = createrules ReduceKsnp
Graphical = Illustrator/GUI
USUAL = lsts2ag \
	ag2lsts \
	ag2tp \
	detlsts \
	parallel \
	extendedrules \
	SBcompare \
	SBreduction \
	BBreduction \
	TauReduction \
	InputLanguage \
	simulate \
	unfold \
	testerrules \
	Illustrator

SUBDIRS = $(SYSDEPS) $(GRAPHICAL) $(USUAL)

tvt: start libtvt.a tplib sysdeps graphical usual lsts_ed

start:
	@echo Compiling the TVT tool.
	@echo It will take approximately half an hour so please be patient.
	@echo ---
	@echo Compiling the TVT library.

tplib:
	@$(MAKE) -C ThirdPartySoftware

# Subdirectories compilation:

%.subdir: %
	@$(MAKE) -C $^

sysdeps: $(SYSDEPS:%=%.subdir)

graphical: $(GRAPHICAL:%=%.subdir)

usual: $(USUAL:%=%.subdir)
	@echo ---
	@echo The compilation of the TVT tool finished.

lsts_ed:
	@$(MAKE) -C gui tvt.lstseditor

# The TVT library construction:

include FileFormats/sources
include FrameWork/sources
include ff/sources

SOURCES = $(FILEFORMATS_SOURCES:%.cc=FileFormats/%.cc) \
	$(FRAMEWORK_SOURCES:%.cc=FrameWork/%.cc) \
	$(NEW_FF_SOURCES:%.cc=ff/%.cc)

include BaseMakefile

LIBTVT_OBJECTS = $(SOURCES:%.cc=%.o)

$(STATIC_TVTLIB): $(LIBTVT_OBJECTS)
	@echo Creating $@ static library.
	@$(AR) r $@ $^
	@echo Done.

$(DYNAMIC_TVTLIB): $(STATIC_TVTLIB)
	@echo Creating $@ dynamic library.
	@$(LD) -G $^ -o $@
	@echo Done.

# Updating all the dependency files:

%.dep_subdir: %
	@$(MAKE) -C $^ dep

updateDeps: $(SUBDIRS:%=%.dep_subdir)

Dep: dep
	@$(MAKE) updateDeps

# Putting TVT into publishable condition:

%.pub_subdir: %
	$(MAKE) -C $^ $(DEP_FILE)

publish: Dep
	$(MAKE) -C ThirdPartySoftware publish

# Misc:

# This one works in Unix/Linux only:
Clean:
	find . \( -name '*.o' -o -name '*.a' -o -name '*.so' \) -exec rm {} \;

.PHONY: tvt start tplib sysdeps graphical usual lsts_ed updateDeps Dep \
	publish Clean
