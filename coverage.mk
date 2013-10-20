.PHONY: coverage genlcov coverage-clean

coverage:
	-$(MAKE) $(AM_MAKEFLAGS) -k check
	$(MAKE) $(AM_MAKEFLAGS) genlcov

infos = libcmis-lcov.info libcmis-c-lcov.info
$(infos): %-lcov.info:
	$(LCOV) --directory $(top_builddir)/src/$* \
		    --base-directory $(top_builddir)/src/$* \
			--capture \
			--output-file $@ \
			--no-external \
		   	--compat-libtool
genlcov: $(infos)
	LANG=C $(GENHTML) --output-directory libcmis-lcov --title "Libcmis Code Coverage" --legend --show-details $^
	@echo "file://$(abs_top_builddir)/libcmis-lcov/index.html"

coverage-clean:
	-$(LCOV) --directory $(top_builddir) -z
	-rm -rf $(infos) libcmis-lcov
