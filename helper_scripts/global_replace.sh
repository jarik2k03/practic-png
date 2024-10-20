#!/bin/sh

STR_CASE=""
STR_FEXT=""

if [ "$#" -ne 5 ]; then
  echo "Usage: $0 <\"recursive_path\"> <\"to_find_string\"> <\"to_replace_found_string\"> <sensetive/insensetive> <cxxmodules/cmakelists>" && exit
fi

if [ "$4" = "sensetive" ]; then
 STR_CASE=""
elif [ "$4" = "insensetive" ]; then
 STR_CASE="I"
else
 echo "Wrong fourth argument!" && exit
fi 

if [ "$5" = "cxxmodules" ]; then
  STR_FEXT="*.cxx"
elif [ "$5" = "cmakelists" ]; then
  STR_FEXT="CMakeLists.txt"
else
 echo "Wrong fifth argument!" && exit
fi

find "$1" -type f -name "$STR_FEXT" -exec sed -i "s/$2/$3/g$STR_CASE" {} \;
