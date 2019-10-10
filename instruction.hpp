#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP
#include <iostream>
#include <cstdint>    //using
#include<bitset>
using word = uint32_t;

class instruction{
public:
  int opcode;
  int src_s, src_t, destn;
  int shamt;
  int funct;
  int i_imi;
  int j_add;
  char type;
  std::bitset<32> bin;
  instruction();
  instruction(word inst);
private:
  bool is_R_type(int opcode);
  bool is_I_type(int opcode);
  bool is_J_type(int opcode);
};

std::ostream& operator<<(std::ostream&  os,const instruction& instr)
{
	os << "opcode:" << "src_s:" << "src_t:" << "destn:"<<"shamt:" <<"funct:"<< "i_imi"<<"j_add"<< "type";
}


#endif
