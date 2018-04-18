TARGETS = 

CLEANFILES = *.O $(TARGETS)


all: $(TARGETS)

clean:
	for %f in ($(CLEANFILES)) DO del %f


