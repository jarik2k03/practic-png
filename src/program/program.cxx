#ifndef NDEBUG
#include <iostream>
#endif

#include <bits/stl_construct.h>
#include <string_view>
import csc.png;
int main(void) {
  csc::deserializer png_executor;
  const csc::png_t file = png_executor.deserialize("vk_small.png");
#ifndef NDEBUG
  std::cout << file << '\n';
#endif
}
