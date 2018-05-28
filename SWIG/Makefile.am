EXTRA_DIST=win32.mak mingw.mak
SUBDIRS = csharp java php5 php7 python python3 


macdist:
	cd csharp ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd java   ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd php5    ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd php7    ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd python2 ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
	cd python3 ; make macdist MACDISTDIR=$(MACDISTDIR) ; cd ..
