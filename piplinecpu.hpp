#pragma once
#include"cpu.hpp"

class piplinecpu :public cpu {
public: // change to private


	s_word rs;
	s_word rt;
public:
	piplinecpu();
	piplinecpu(std::string binary);

	void run5stage();
	//TEST:

public:
	
	word IF();//IFetch
	instruction ID(word next_instruction);//Idecode
	void EX(const instruction&);//Execute
	void Mem();//MemAccess	

	void WB();

	//INSTRUCTIONS
	void ADD(const instruction& inst);
	void ADDI(const instruction& inst);
	void ADDIU(const instruction& inst);
	void ADDU(const instruction& inst);
	void AND(const instruction& inst);
	void ANDI(const instruction& inst);
	void BEQ(const instruction& inst);
	void BGEZ(const instruction& inst);
	void BGEZAL(const instruction& inst);
	void BGTZ(const instruction& inst);
	void BLEZ(const instruction& inst);
	void BLTZ(const instruction& inst);
	void BLTZAL(const instruction& inst);
	void BNE(const instruction& inst);
	void DIV(const instruction& inst);
	void DIVU(const instruction& inst);
	void J(const instruction& inst);
	void JALR(const instruction& inst);
	void JAL(const instruction& inst);
	void JR(const instruction& inst);
	void LB(const instruction& inst);
	void LBU(const instruction& inst);
	void LH(const instruction& inst);
	void LHU(const instruction& inst);
	void LUI(const instruction& inst);
	void LW(const instruction& inst);
	void LWL(const instruction& inst);
	void LWR(const instruction& inst);
	void MFHI(const instruction& inst);
	void MFLO(const instruction& inst);
	void MTHI(const instruction& inst);
	void MTLO(const instruction& inst);
	void MULT(const instruction& inst);
	void MULTU(const instruction& inst);
	void OR(const instruction& inst);
	void ORI(const instruction& inst);
	void SB(const instruction& inst);
	void SH(const instruction& inst);
	void SLL(const instruction& inst);
	void SLLV(const instruction& inst);
	void SLT(const instruction& inst);
	void SLTI(const instruction& inst);
	void SLTIU(const instruction& inst);
	void SLTU(const instruction& inst);
	void SRA(const instruction& inst);
	void SRAV(const instruction& inst);
	void SRL(const instruction& inst);
	void SRLV(const instruction& inst);
	void SUB(const instruction& inst);
	void SUBU(const instruction& inst);
	void SW(const instruction& inst);
	void XOR(const instruction& inst);
	void XORI(const instruction& inst);


};
