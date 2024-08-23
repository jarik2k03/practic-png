#!/bin/sh

if [ -n "$2" ]
  find src/csc -type f -exec sed --debug -i 's/$1/$2/gI' {} \;
else
  cat "1arg: to_find_string, 2arg: to_replace_found_string"
