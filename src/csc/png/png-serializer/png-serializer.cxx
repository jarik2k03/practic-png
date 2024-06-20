module;
#include <stdio.h>
 
export module csc.png:serializer;
import csc.png.sections;

void serialize() {
  printf("serialized!\n");
}
