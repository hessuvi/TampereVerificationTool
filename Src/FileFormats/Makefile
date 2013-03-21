TVT_BASE = ..

include sources

SOURCES = $(FILEFORMATS_SOURCES)

include $(TVT_BASE)/BaseMakefile

tidy:
	$(RM) $(FF:%=%/*.o)
	$(RM) $(LEX:%=%/*.o)
	$(RM) $(LSTS_FILE:%=%/*.o)
	$(RM) $(LSTS_SECS:%=%/*.o)
	$(RM) $(PAR_RULES_FILE:%=%/*.o)
	$(RM) $(PAR_SECS:%=%/*.o)
