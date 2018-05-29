JSONTRY = ../jsontry

#
# These must all start with the prefix "test", so as to work
# with KNOWN_GOOD_OUTPUT below.
#
MYINPUT = test1.json test2.json test3.json \
          test1.error-json  test2.error-json  test3.error-json 

MYOUTPUT = $(MYINPUT:json=txt)

MYOUTPUT_STATIC = $(MYOUTPUT) 

KNOWN_GOOD_OUTPUT = known_good_output/test*

CLEANFILES = *~ *.pyc $(MYOUTPUT) diff.txt

EXTRA_DIST = $(MYINPUT) $(KNOWN_GOOD_OUTPUT) \
             expect_false.sh

.PHONY: test
test: clean diff.txt

.error-json.error-txt:
	$(SH) ./expect_false.sh $(JSONTRY) -d $< > $@ 2>&1

.json.txt:
	$(JSONTRY) -d $< > $@ 2>&1

SUFFIXES = .error-json .error-txt .json .txt

diff.txt: $(MYOUTPUT)
	rm -f $@
	for f in $(MYOUTPUT); do diff $$f known_good_output/$$f >> $@; done
	test -z "`cat $@`"
	@echo ""
	@echo "All JSON tests passed."
	@echo ""

