#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include "memory.hpp"
#include "instruction.hpp"
#include "registers.hpp"

using word = uint32_t;
using s_word = int32_t;

class cpu{
public: // change to private
  memory m;
  registers r;
  word pc;
  word npc;

  word LO;
  word HI;

public:
  cpu();
  cpu(std::string binary);
  void run();//single

  //TEST:
  void test_fill();
  void reg_print(bool s_nbr);
  void reg_s();
protected:
  void execute  (const instruction& inst);

  virtual void execute_r(const instruction& inst);
  virtual void execute_i(const instruction& inst);
  virtual void execute_j(const instruction& inst);


  //INSTRUCTIONS
  virtual void ADD   (const instruction& inst);
  virtual void ADDI  (const instruction& inst);
  virtual void ADDIU (const instruction& inst);
  virtual void ADDU  (const instruction& inst);
  virtual void AND   (const instruction& inst);
  virtual void ANDI  (const instruction& inst);
  virtual void BEQ   (const instruction& inst);
  virtual void BGEZ  (const instruction& inst);
  virtual void BGEZAL(const instruction& inst);
  virtual void BGTZ  (const instruction& inst);
  virtual void BLEZ  (const instruction& inst);
  virtual void BLTZ  (const instruction& inst);
  virtual void BLTZAL(const instruction& inst);
  virtual void BNE   (const instruction& inst);
  virtual void DIV   (const instruction& inst);
  virtual void DIVU  (const instruction& inst);
  virtual void J     (const instruction& inst);
  virtual void JALR  (const instruction& inst);
  virtual void JAL   (const instruction& inst);
  virtual void JR    (const instruction& inst);
  virtual void LB    (const instruction& inst);
  virtual void LBU   (const instruction& inst);
  virtual void LH    (const instruction& inst);
  virtual void LHU   (const instruction& inst);
  virtual void LUI   (const instruction& inst);
  virtual void LW    (const instruction& inst);
  virtual void LWL   (const instruction& inst);
  virtual void LWR   (const instruction& inst);
  virtual void MFHI  (const instruction& inst);
  virtual void MFLO  (const instruction& inst);
  virtual void MTHI  (const instruction& inst);
  virtual void MTLO  (const instruction& inst);
  virtual void MULT  (const instruction& inst);
  virtual void MULTU (const instruction& inst);
  virtual void OR    (const instruction& inst);
  virtual void ORI   (const instruction& inst);
  virtual void SB    (const instruction& inst);
  virtual void SH    (const instruction& inst);
  virtual void SLL   (const instruction& inst);
  virtual void SLLV  (const instruction& inst);
  virtual void SLT   (const instruction& inst);
  virtual void SLTI  (const instruction& inst);
  virtual void SLTIU (const instruction& inst);
  virtual void SLTU  (const instruction& inst);
  virtual void SRA   (const instruction& inst);
  virtual void SRAV  (const instruction& inst);
  virtual void SRL   (const instruction& inst);
  virtual void SRLV  (const instruction& inst);
  virtual void SUB   (const instruction& inst);
  virtual void SUBU  (const instruction& inst);
  virtual void SW    (const instruction& inst);
  virtual void XOR   (const instruction& inst);
  virtual void XORI  (const instruction& inst);

  //SUB-INSTRUCTION
  void pc_increase(word offset);
  word sign_extend_imi(const instruction& inst);
  bool test_overflow(word a, word b, word res);
  void test_zero_fields_R(const instruction& inst);
  void test_zero_fields_I(const instruction& inst);
};




#endif
