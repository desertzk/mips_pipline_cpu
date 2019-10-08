#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>    //using

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

  instruction();
  instruction(word inst);
private:
  bool is_R_type(int opcode);
  bool is_I_type(int opcode);
  bool is_J_type(int opcode);
};

#endif
