#include <cstdint> //using
#include <stdexcept> //std::exit
#include <iostream> //debug
#include "instruction.hpp"

using word = uint32_t;
using namespace std;

std::ostream& operator<<(std::ostream&  os, const instruction& instr)
{
	std::string binstr = instr.bin.to_string();
	if (instr.type == 'r')
		os << "R   " << "opcode:" << binstr.substr(0, 6) << "rs:" << binstr.substr(6, 5) << "rt:" << binstr.substr(11, 5) << "rd:" << binstr.substr(16, 5) << "shamt:" << binstr.substr(21, 5) << "funct:" << binstr.substr(26, 6) << "  ";
	else if (instr.type == 'i')
		os << "I   " << "opcode:" << binstr.substr(0, 6) << "rs:" << binstr.substr(6, 5) << "rt:" << binstr.substr(11, 5) << "address offset:" << binstr.substr(16, 16) <<"i_imi" << instr.i_imi << "  ";
	else
		os << "J   " << "opcode:" << binstr.substr(0, 6) << "target address:"<<binstr.substr(6,26);
	return os;
}


instruction::instruction(){
  opcode = 0;
  src_s = 0;
  src_t = 0;
  destn = 0;
  shamt = 0;
  funct = 0;
  i_imi = 0;
  j_add = 0;
}

instruction::instruction(word inst):bin(inst){
  opcode = inst >> 26;
  if(is_R_type(opcode)){
    type = 'r';
    src_s = (inst >> 21) & 0x1F; //$rs
    src_t = (inst >> 16) & 0x1F; //$rt
    destn = (inst >> 11) & 0x1F; //$rd
    shamt = (inst >> 6 ) & 0x1F;
    funct =  inst & 0x3F;
  }else if(is_I_type(opcode)){
    type = 'i';
    src_s = (inst >> 21) & 0x1F;//取5bit
    src_t = (inst >> 16) & 0x1F;
    i_imi =  inst & 0xFFFF;
  }else if(is_J_type(opcode)){
    type = 'j';
    j_add =  inst & 0x3FFFFFF;
  }else{
    std::cerr<<"error: opcode <" << opcode <<  "> not valid / implemented"<<std::endl;
    std::exit(-12);
  }
}

bool instruction::is_R_type(int opcode){
  return opcode == 0x00;
}
bool instruction::is_I_type(int opcode){
  return opcode == 0x01 || opcode == 0x04 || opcode == 0x05 || opcode == 0x06 || opcode == 0x07 ||
         opcode == 0x08 || opcode == 0x09 || opcode == 0x0A || opcode == 0x0B || opcode == 0x0C ||
         opcode == 0x0D || opcode == 0x0E || opcode == 0x0F || opcode == 0x20 || opcode == 0x21 ||
         opcode == 0x22 || opcode == 0x23 || opcode == 0x24 || opcode == 0x25 || opcode == 0x26 ||
         opcode == 0x28 || opcode == 0x29 || opcode == 0x2B;
}
bool instruction::is_J_type(int opcode){
  return opcode == 0x02 || opcode == 0x03;
}
