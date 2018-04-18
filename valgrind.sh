#!/bin/bash
valgrind --trace-children=yes --leak-check=full --leak-resolution=high --show-reachable=yes --track-origins=yes -v $*  > out.txt 2>valgrind.out.txt
echo "Done. Check out.txt and valgrind.out.txt."
