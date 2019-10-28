#pragma once
#include"cpu.hpp"
#include<thread>
#include<queue>
#include <mutex>

class piplinecpu :public cpu {
public: // change to private


	
public:
	piplinecpu();
	piplinecpu(std::string binary);

	void run5stage();
	//TEST:

public:
	long long cycle;
	long long ifcycle;
	void CycleControl();
	void IF();//IFetch
	void ID();//Idecode
	void EX();//Execute
	void Mem();//MemAccess
	
	void WB();
	std::mutex mutcycle;
	std::mutex mutcycledone;
	std::condition_variable cvcycle;
	std::condition_variable cvcycledone;

	std::queue<word> queue_if;
	std::mutex mutif;
	//std::condition_variable cvcycle;

	//instruction rs rt
	std::queue<std::tuple<instruction, s_word, s_word>> queue_id;
	std::mutex mutid;
	//std::condition_variable cvid;

	std::queue<instruction> queue_ex;
	std::mutex mutex;
	//std::condition_variable cvex;
	//std::queue<word> queue_ex;

	std::queue<std::tuple<std::string,word,word>> queue_mem;
	std::mutex mutmem;
	//std::condition_variable cvmem;

	std::queue<word> queue_wb;
	//std::mutex mutwb;
	//std::condition_variable cvwb;
	virtual void execute_r(const instruction& inst, s_word rs, s_word rt);
	virtual void execute_i(const instruction& inst, s_word rs, s_word rt);
	virtual void execute_j(const instruction& inst, s_word rs, s_word rt);

    bool is_ex=false;

	//INSTRUCTIONS
	void ADD(const instruction& inst, s_word rs, s_word rt);
	void ADDI(const instruction& inst, s_word rs, s_word rt);
	void ADDIU(const instruction& inst, s_word rs, s_word rt);
	void ADDU(const instruction& inst, s_word rs, s_word rt);
	void AND(const instruction& inst, s_word rs, s_word rt);
	void ANDI(const instruction& inst, s_word rs, s_word rt);
	void BEQ(const instruction& inst, s_word rs, s_word rt);
	void BGEZ(const instruction& inst, s_word rs, s_word rt);
	void BGEZAL(const instruction& inst, s_word rs, s_word rt);
	void BGTZ(const instruction& inst, s_word rs, s_word rt);
	void BLEZ(const instruction& inst, s_word rs, s_word rt);
	void BLTZ(const instruction& inst, s_word rs, s_word rt);
	void BLTZAL(const instruction& inst, s_word rs, s_word rt);
	void BNE(const instruction& inst, s_word rs, s_word rt);
	void DIV(const instruction& inst, s_word rs, s_word rt);
	void DIVU(const instruction& inst, s_word rs, s_word rt);
	void J(const instruction& inst, s_word rs, s_word rt);
	void JALR(const instruction& inst, s_word rs, s_word rt);
	void JAL(const instruction& inst, s_word rs, s_word rt);
	void JR(const instruction& inst, s_word rs, s_word rt);
	void LB(const instruction& inst, s_word rs, s_word rt);
	void LBU(const instruction& inst, s_word rs, s_word rt);
	void LH(const instruction& inst, s_word rs, s_word rt);
	void LH_mem(word res);
	void LH_wb(int src_t, word res);

	void LHU(const instruction& inst, s_word rs, s_word rt);
	void LUI(const instruction& inst, s_word rs, s_word rt);
	void LW(const instruction& inst, s_word rs, s_word rt);
	void LWL(const instruction& inst, s_word rs, s_word rt);
	void LWR(const instruction& inst, s_word rs, s_word rt);
	void MFHI(const instruction& inst, s_word rs, s_word rt);
	void MFLO(const instruction& inst, s_word rs, s_word rt);
	void MTHI(const instruction& inst, s_word rs, s_word rt);
	void MTLO(const instruction& inst, s_word rs, s_word rt);
	void MULT(const instruction& inst, s_word rs, s_word rt);
	void MULTU(const instruction& inst, s_word rs, s_word rt);
	void OR(const instruction& inst, s_word rs, s_word rt);
	void ORI(const instruction& inst, s_word rs, s_word rt);
	void SB(const instruction& inst, s_word rs, s_word rt);
	void SH(const instruction& inst, s_word rs, s_word rt);
	void SLL(const instruction& inst, s_word rs, s_word rt);
	void SLLV(const instruction& inst, s_word rs, s_word rt);
	void SLT(const instruction& inst, s_word rs, s_word rt);
	void SLTI(const instruction& inst, s_word rs, s_word rt);
	void SLTIU(const instruction& inst, s_word rs, s_word rt);
	void SLTU(const instruction& inst, s_word rs, s_word rt);
	void SRA(const instruction& inst, s_word rs, s_word rt);
	void SRAV(const instruction& inst, s_word rs, s_word rt);
	void SRL(const instruction& inst, s_word rs, s_word rt);
	void SRLV(const instruction& inst, s_word rs, s_word rt);
	void SUB(const instruction& inst, s_word rs, s_word rt);
	void SUBU(const instruction& inst, s_word rs, s_word rt);

	void SW(const instruction& inst, s_word rs, s_word rt);
	void XOR(const instruction& inst, s_word rs, s_word rt);
	void XORI(const instruction& inst, s_word rs, s_word rt);

	bool is_finish;
};
