DEFINE_SQLITE_EMDFDB_NO_FSYNC = -DSQLITE_EMDFDB_NO_FSYNC
#DEFINE_SQLITE_EMDFDB_NO_FSYNC = 


CXXFLAGS=-I../include $(DEFINE_SQLITE_EMDF_NO_FSYNC) -O3
#CXXFLAGS=-I../include $(DEFINE_SQLITE_EMDF_NO_FSYNC) -g


#
# Sources
#

emdflib_src = arena.cpp \
       conn.cpp \
       emdfdb.cpp \
       emdf_ffeatures.cpp \
       emdf_hash.cpp \
       emdf_output.cpp \
       emdf_value.cpp \
       encryption.cpp \
       enum_const_cache.cpp \
       crc32.cpp \
       emdf_primes.cpp \
       infos.cpp \
       inst.cpp \
       inst_object.cpp \
       logging.cpp \
       monads.cpp \
       string_func.cpp \
       string2som_map.cpp \
       string_list.cpp \
       string_set_cache.cpp \
       table.cpp \
       utils.cpp \
       sqliteconn.cpp \
       sqliteemdfdb.cpp

#
# Objects
#
objects = $(emdflib_src:.cpp=.o)

CLEANFILES = *. O  $(TARGETS)

TARGETS = libemdf.a

all: $(TARGETS)

libemdf.a: $(objects)
	ar r $@ $(objects)

.CPP.O:
	gcc $(CXXFLAGS) -c $<

        

clean:
	for %f in ($(CLEANFILES)) DO del %f

