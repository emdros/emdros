EXTRA_DIST=win32.mak mingw.mak

SUBDIRS = csharp java php7 python python3


macdist:
	cd csharp ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd java   ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd php7    ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd python ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd python3 ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
