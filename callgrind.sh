#!/bin/bash
valgrind --tool=callgrind --collect-systime=no -v $*  > out.txt 2>callgrind.err.txt
PROCID=`tail -n 1 callgrind.err.txt | awk '{print $1}' | sed -e 's_=__g'`
ANN_TREE_NONE_INCLUSIVE_NO="callgrind.out.$PROCID.tree_none.inclusive_no.txt"
ANN_TREE_NONE_INCLUSIVE_YES="callgrind.out.$PROCID.tree_none.inclusive_yes.txt"
ANN_TREE_BOTH_INCLUSIVE_NO="callgrind.out.$PROCID.tree_both.inclusive_no.txt"
ANN_TREE_BOTH_INCLUSIVE_YES="callgrind.out.$PROCID.tree_both.inclusive_yes.txt"
INFILE="callgrind.out.$PROCID"
callgrind_annotate --tree=none --inclusive=no --auto=yes $INFILE > $ANN_TREE_NONE_INCLUSIVE_NO
callgrind_annotate --tree=none --inclusive=yes --auto=yes $INFILE > $ANN_TREE_NONE_INCLUSIVE_YES
callgrind_annotate --tree=both --inclusive=no --auto=yes $INFILE > $ANN_TREE_BOTH_INCLUSIVE_NO
callgrind_annotate --tree=both --inclusive=yes --auto=yes $INFILE > $ANN_TREE_BOTH_INCLUSIVE_YES

echo "Done. Check the following files:"
echo "   - out.txt (stdout output)"
echo "   - callgrind.err.txt (stderr output)"
echo "   - $ANN_TREE_NONE_INCLUSIVE_NO"
echo "   - $ANN_TREE_NONE_INCLUSIVE_YES"
echo "   - $ANN_TREE_BOTH_INCLUSIVE_NO"
echo "   - $ANN_TREE_BOTH_INCLUSIVE_YES"

