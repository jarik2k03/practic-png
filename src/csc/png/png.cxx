module;
#include <stdio.h>
export module csc.png;
export import :deserializer;
export import :serializer;
// #include <iostream>


consteval int hidden() {
  return 42;
}

export void print() {
  printf( "VALUE: %d\n", hidden()); 
  serialize();
  csc::deserialize();
}
