AM_CFLAGS = @ADDITIONAL_CFLAGS@

ACLOCAL_AMFLAGS = -I m4

if WITH_WX_PROGRAMS
  MAYBE_WX=wx
endif

EMDROS_SPEC_FILE = emdros-spec-@VERSION@.spec

EXTRA_DIST = README INSTALL.Win32.txt INSTALL.Mac.txt \
             Doxyfile autogen.sh rmfiles.sh install-sh \
             emdros.spec.in $(EMDROS_SPEC_FILE) \
             generate_extrafiles.py.in \
             install_mac.sh.in signthis.bat \
             README-Emdros-on-Android.txt \
             README-Emdros-on-iOS.txt \
             valgrind.sh callgrind.sh \
             debian/changelog \
             debian/compat \
             debian/control \
             debian/copyright \
             debian/docs \
             debian/docs.in \
             debian/emdros.examples \
             debian/emdros.manpages \
             debian/menu \
             debian/rules   debian/rules.in \
             debian/emdros.postinst   debian/emdros.postinst.in \
             debian/emdros.postrm   debian/emdros.postrm.in \
             debian/emdros.prerm   debian/emdros.prerm.in \
             debian/emdros.doc-base   debian/docbase.in \
             debian/source/format 



SUBDIRS = doc pcre zlib sqlite3 include \
          EMdF MQL harvest util amalgamation $(MAYBE_WX) \
          src tests examples \
	  SWIG win32

CLEANFILES = *~ core install_mac.sh generate_extrafiles.py extrafiles-emdros.txt extrafiles-emdros-devel.txt

DISTCLEANFILES = debian/docs \
                 $(EMDROS_SPEC_FILE) $(EMDROS_SPEC_FILE).in

BUILT_SOURCES = install_mac.sh debian/docs generate_extrafiles.py extrafiles-emdros.txt $(EMDROS_SPEC_FILE) \
                debian/rules \
                debian/emdros.postinst \
                debian/emdros.postrm \
                debian/emdros.prerm \
                debian/emdros.doc-base

include include/Makefile.inc



generate_extrafiles.py: generate_extrafiles.py.in config.status
	$(top_builddir)/config.status --file=$@

extrafiles-emdros.txt: generate_extrafiles.py
	python generate_extrafiles.py



install_mac.sh: install_mac.sh.in config.status 
	$(top_builddir)/config.status --file=./install_mac.sh
	chmod 755 ./install_mac.sh

debian/rules: debian/rules.in  config.status
	$(top_builddir)/config.status --file=$@
	chmod 755 $@

debian/emdros.postrm: debian/emdros.postrm.in  config.status
	$(top_builddir)/config.status --file=$@

debian/emdros.prerm: debian/emdros.prerm.in  config.status
	$(top_builddir)/config.status --file=$@

debian/emdros.postinst: debian/emdros.postinst.in  config.status
	$(top_builddir)/config.status --file=$@

debian/emdros.doc-base: debian/docbase.in  config.status
	cp debian/docbase.in  debian/emdros.doc-base.in
	$(top_builddir)/config.status --file=$@
	rm -f debian/emdros.doc-base.in


$(EMDROS_SPEC_FILE): emdros.spec.in config.status extrafiles-emdros.txt
	cp emdros.spec.in ./$(EMDROS_SPEC_FILE).in
	$(top_builddir)/config.status --file=./$(EMDROS_SPEC_FILE)
	rm -f ./$(EMDROS_SPEC_FILE).in
	chmod 664 ./$(EMDROS_SPEC_FILE)

debian/docs: debian/docs.in $(top_builddir)/config.status 
	$(top_builddir)/config.status --file=$@
	chmod 664 $@




AM_CPPFLAGS = -I$(top_srcdir)/include


MACDISTDIR = Emdros-@VERSION@-MacOSX-@MACOSX_VERSION_MIN@.@MACCPU@
MACDMG = $(MACDISTDIR).dmg


macdist: all
	rm -f $(MACDMG)
	rm -rf $(MACDISTDIR)
	mkdir -p $(MACDISTDIR)
	ln -s /Applications $(MACDISTDIR)/
	mkdir -p $(MACDISTDIR)/development
	mkdir -p $(MACDISTDIR)/development/bin
	mkdir -p $(MACDISTDIR)/development/include
	mkdir -p $(MACDISTDIR)/development/man/1
	mkdir -p $(MACDISTDIR)/development/lib
	mkdir -p $(MACDISTDIR)/development/share/emdros/hal
	mkdir -p $(MACDISTDIR)/doc
	mkdir -p $(MACDISTDIR)/doc/PDF
	mkdir -p $(MACDISTDIR)/doc/txt
	mkdir -p $(MACDISTDIR)/doc/html
if SWIG_WITH_CSHARP_WRAPPERS
	mkdir -p $(MACDISTDIR)/development/SWIG/csharp
	mkdir -p $(MACDISTDIR)/development/SWIG/csharp
endif
if SWIG_WITH_JAVA_WRAPPERS
	mkdir -p $(MACDISTDIR)/development/SWIG/java
endif
if SWIG_WITH_PERL_WRAPPERS
	mkdir -p $(MACDISTDIR)/development/SWIG/perl
endif
if SWIG_WITH_PYTHON_WRAPPERS
	mkdir -p $(MACDISTDIR)/development/SWIG/python
endif
if SWIG_WITH_PYTHON3_WRAPPERS
	mkdir -p $(MACDISTDIR)/development/SWIG/python3
endif
if SWIG_WITH_PHP_WRAPPERS
	mkdir -p $(MACDISTDIR)/development/SWIG/php5
endif
if SWIG_WITH_PHP7_WRAPPERS
	mkdir -p $(MACDISTDIR)/development/SWIG/php7
endif
if SWIG_WITH_RUBY_WRAPPERS
	mkdir -p $(MACDISTDIR)/development/SWIG/ruby
endif
	mkdir -p $(MACDISTDIR)/development/sourcecode
if WX_DO_UNICODE
	cp -Rp src/qrytool/EmdrosQueryTool.app $(MACDISTDIR)/
	cp -Rp src/importtool/EmdrosImportTool.app $(MACDISTDIR)/
	cp -Rp examples/ChunkingTool/EmdrosChunkingTool.app $(MACDISTDIR)/
	cp examples/ChunkingTool/doc/ChunkingToolGuide.pdf $(MACDISTDIR)/doc/PDF/
	cp src/qrytool/TECkit/TECkit_Compile $(MACDISTDIR)/development/bin
	cp src/qrytool/TECkit/TxtConv $(MACDISTDIR)/development/bin
	cp src/qrytool/TECkit/.libs/libteckit.a $(MACDISTDIR)/development/lib
	cp src/qrytool/doc/EQTUsersGuide.pdf $(MACDISTDIR)/doc/PDF
endif
	cd doc ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd src ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd examples/HAL ; make macdist MACDISTDIR=../../$(MACDISTDIR) ; cd ../.\.
	cd SWIG ; make macdist MACDISTDIR=../../$(MACDISTDIR) ; cd ..
	cp ../emdros-@VERSION@.tar.gz $(MACDISTDIR)/development/sourcecode/
	cd EMdF ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd MQL ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd util ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd wx ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd sqlite3 ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd pcre ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd tests ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cd include ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
	cp COPYING $(MACDISTDIR)/COPYING.txt
	cp README $(MACDISTDIR)/README.txt
	cp INSTALL.Mac.txt $(MACDISTDIR)/INSTALL.Mac.txt
	cp AUTHORS $(MACDISTDIR)/doc/AUTHORS.txt
	cp NEWS $(MACDISTDIR)/doc/NEWS.txt
	cp ChangeLog $(MACDISTDIR)/doc/ChangeLog.txt
	cp install_mac.sh $(MACDISTDIR)/development/
	hdiutil create -srcfolder $(MACDISTDIR) -fs HFS+ $(MACDMG)


test:
	$(MAKE) -C tests test

EMDROS_EXAMPLES_VERSION = 2.0
EXAMPLESDIR = Emdros-Examples-$(EMDROS_EXAMPLES_VERSION)
EXAMPLESZIP = $(EXAMPLESDIR).zip

examples: $(EXAMPLESZIP)

$(EXAMPLESZIP): all
	rm -f $(EXAMPLESZIP)
	rm -rf $(EXAMPLESDIR)
	mkdir -p $(EXAMPLESDIR)
	$(MAKE) -C examples examples EXAMPLESDIR=../$(EXAMPLESDIR)
	zip -r $(EXAMPLESZIP) $(EXAMPLESDIR)
	@echo "Now look in " $(EXAMPLESZIP)


clean-local:
	rm -f $(EMDROS_SPEC_FILE)
	rm -f $(EXAMPLESZIP)
	rm -rf $(EXAMPLESDIR)
