include ../.makefile_vars
-include ../comp_settings

lp_solve/liblpk.a: lp_solve/Makefile
	$(MAKE) -C lp_solve -f ../Makefile.lp_solve

lp_solve/Makefile:
	cd lp_solve; gunzip -c lp_solve_3.2.tar.gz | $(TAR) xf -

clean:
	$(RM) lp_solve/*.o 
	$(RM) lp_solve/*.a

publish: lp_solve/Makefile
	$(RM) lp_solve/*.tar.gz

.PHONY: clean publish
