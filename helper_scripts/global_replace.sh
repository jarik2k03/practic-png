#!/bin/sh

STR_CASE=""

if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <\"to_find_string\"> <\"to_replace_found_string\"> <sensetive/insensetive>" && exit
fi

if [ "$3" = "sensetive" ]; then
 STR_CASE = ""
elif [ "$3" = "insensetive" ]; then 
 STR_CASE = "I"
else
 echo "Wrong third argument!" && exit
fi 

find src/csc -type f -name "*.cxx" -exec sed --debug -i "s/$1/$2/g$STR_CASE" {} \;
