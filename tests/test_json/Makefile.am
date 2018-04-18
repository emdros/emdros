JSONTRY = ../jsontry

#
# These must all start with the prefix "test", so as to work
# with KNOWN_GOOD_OUTPUT below.
#
MYINPUT = test1.json test2.json test3.json \
          testerror1.json  testerror2.json  testerror3.json 

MYOUTPUT = $(MYINPUT:.json=.txt)

MYOUTPUT_STATIC = $(MYOUTPUT) 

KNOWN_GOOD_OUTPUT = $(subst test,known_good_output/test,$(MYOUTPUT_STATIC))

CLEANFILES = *~ *.pyc $(MYOUTPUT) diff.txt

EXTRA_DIST = $(MYINPUT) $(KNOWN_GOOD_OUTPUT) \
             expect_false.sh

.PHONY: test
test:
	$(MAKE) clean
	$(MAKE) diff.txt

testerror%.txt: testerror%.json
	$(SH) ./expect_false.sh $(JSONTRY) -d $< > $@ 2>&1

%.txt: %.json
	$(JSONTRY) -d $< > $@ 2>&1


diff.txt: $(MYOUTPUT)
	rm -f $@
	for f in $(MYOUTPUT); do diff $$f known_good_output/$$f >> $@; done
	test -z "`cat $@`"
	@echo ""
	@echo "All JSON tests passed."
	@echo ""

