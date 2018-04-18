#!/bin/bash
for d in `ruby -e 'puts $:.join("\n")'`; do
  if test -f $d/ruby.h; then
    echo $d;
    break;
  fi
done
