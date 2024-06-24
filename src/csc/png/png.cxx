module;
#include <stdio.h>
export module csc.png;
export import :deserializer;

import csc.png.sections;
// export import :serializer;


consteval int hidden() {
  return 42;
}

export void print() {
  printf( "VALUE: %d\n", hidden()); 
  // serialize();
  csc::deserialize();
}
