#include "registers.hpp"

using word = uint32_t;

registers::registers(){
  dummy = 0;
  for(int i = 0; i < 32; i++)
    reg[i] = 0;
}

word registers::get(int i){
  return reg[i];
}

void registers::set(int i, word data){
  if(i != 0) reg[i] = data;
}

word& registers::operator[](int i){
  if(i != 0) return reg[i];
  dummy = 0;
  return dummy;
}

std::ostream& operator<<(std::ostream&  os, const registers& reg)
{
	
	
	return os;
}
