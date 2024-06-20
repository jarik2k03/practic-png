import csc.png;

#include <iostream>
int main(void) {
  csc::print_chunk();
  const auto chank = csc::create_chunk(42); // - скомпилится - chunk виден благодаря возвращаемому значению 
  std::cout << "Шанс конца света к 1052763432 == " << chank.word << "%\n";
  //csc::chunk fuu{}; // - не скомпилится - chunk не виден за пределами png - модуля

}
