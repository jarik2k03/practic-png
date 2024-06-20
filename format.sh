find src/ -iname *.hxx -o -iname *.cxx | xargs clang-format -i
find tests/ -iname *.hxx -o -iname *.cxx | xargs clang-format -i