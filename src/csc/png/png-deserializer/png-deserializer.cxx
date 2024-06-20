module;
#include <stdio.h>

export module csc.png:deserializer;
import csc.png.sections;

export namespace csc {
void print_chunk() {
  csc::chunk ch{0};
  printf("chunk: %d\n", ch.word);
}

auto create_chunk(int val) -> csc::chunk {
  return csc::chunk(val);
}
void deserialize() {
  printf("deserialized!\n");
}




}
