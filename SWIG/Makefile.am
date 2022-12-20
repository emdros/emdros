EXTRA_DIST=win32.mak mingw.mak

SUBDIRS = csharp java php7 php8 python python3 nodejs


macdist:
	cd csharp  ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd java    ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd php7    ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd php8    ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd python  ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd python3 ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd nodejs  ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..


# Recurse into the directories that have a 'gen_files' target, and
# execute that target.
#
# The gen_files target updates Makefile.inc in each of these
# subdirectories.
#
# It is necessary to run gen_files whenever a class is added or
# subtracted from the SWIG generated bindings.
#
gen_files:
	cd csharp  ; make gen_files ; cd ..
	cd java    ; make gen_files ; cd ..

