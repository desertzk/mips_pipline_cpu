#ifndef REGISTERS_HPP
#define REGISTERS_HPP
#include <cstdint>
#include <iostream>
using word = uint32_t;

class registers{
private:
  word reg[32];
  word dummy;
public:
  registers();
  word get(int i);
  void set(int i, word data);
  word& operator[](int i);
};

std::ostream& operator<<(std::ostream&  os, const registers& reg);

#endif
