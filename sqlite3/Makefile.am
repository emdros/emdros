#
# Makefile for SQLITE3
#


if USE_LOCAL_SQLITE3
MYSUBDIRS = tool src
#MY_NON_DISTRIBUTED_SUBDIRS = art doc www 
DO_MACDIST = cd src ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd .. ; cp -f sqlite3_emdros.1 $(MACDISTDIR)/development/man/1
else
MYSUBDIRS = tool
DO_MACDIST = 
endif

SUBDIRS=$(MYSUBDIRS)

EXTRA_DIST = sqlite3_emdros.1 VERSION \
             win32.mak mingw.mak djgpp.mak sqlite3.pro


# Man pages
man1_MANS = sqlite3_emdros.1

macdist:
	$(DO_MACDIST)
