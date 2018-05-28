EXTRA_DIST=win32.mak mingw.mak
SUBDIRS = csharp java php php7 python python3 ruby


macdist:
	cd csharp ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd java   ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd php    ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd php7    ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd python ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd python3 ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd ruby   ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
