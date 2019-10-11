#include"piplinecpu.hpp"

#include <string>
#include <iostream> //debug
#include <stdexcept> //std::exit

#include "instruction.hpp"


using namespace std;

piplinecpu::piplinecpu(): cpu(),rs(0),rt(0){
	pc = 0x10000000;
	npc = 0x10000004;
	LO = 0;
	HI = 0;
}

piplinecpu::piplinecpu(std::string binary) :cpu(binary), rs(0), rt(0) {
	pc = 0x10000000;
	npc = 0x10000004;
	LO = 0;
	HI = 0;
}



void piplinecpu::run5stage() {
	//std::thread t5(&foo::bar, &f); // t5 runs foo::bar() on object f
	while (true) {
		word instruct = this->IF();
		instruction c_inst = this->ID(instruct);
		this->EX(c_inst);

		if (pc == 0) {
			exit(r[2]);
		}
	}
}



word piplinecpu::IF()
{
	word instruction = m.read_inst(pc);
	return instruction;
}

instruction piplinecpu::ID(word next_instruction)
{
	instruction inst(next_instruction);
	rs = r[inst.src_s];
	rt = r[inst.src_t];
	return inst;
}

void piplinecpu::EX(const instruction& inst) {
	cout << inst;
	switch (inst.type) {
	case 'r': execute_r(inst); break;
	case 'i': execute_i(inst); break;
	case 'j': execute_j(inst); break;
	default: std::cerr << "error: incorect instruction type"; std::exit(-20);
	}
}

void piplinecpu::Mem()
{
}

void piplinecpu::WB()
{
	//return 0;
}











// INSTRUCTIONS
void piplinecpu::ADD(const instruction& inst) {
	//id
	s_word r1 = rs;
	s_word r2 = rt;
	//ex
	s_word res = r1 + r2;

	if ((res < 0 && r1 >= 0 && r2 >= 0) || (res >= 0 && r1 < 0 && r2 < 0)) {
		std::cerr << "exception: arithmetic error" << std::endl;
		std::exit(-10);
	}

	r[inst.destn] = res;
	pc_increase(4);
}
void piplinecpu::ADDI(const instruction& inst) {
	//TODO: check immiatde sing extension
	s_word r1 = rs;
	s_word imi = sign_extend_imi(inst);
	s_word res = r1 + imi;

	if ((res <= 0 && r1 > 0 && imi > 0) || (res >= 0 && r1 < 0 && imi < 0)) {
		std::cerr << "exception: arithmetic error" << '\n';
		std::exit(-10);
	}

	r[inst.src_t] = res;
	pc_increase(4);
}
void piplinecpu::ADDIU(const instruction& inst) {
	word r1 = rs;
	word imi = sign_extend_imi(inst);
	s_word res = r1 + imi;
	r[inst.src_t] = res;
	pc_increase(4);
}
void piplinecpu::ADDU(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	word res = r1 + r2;
	r[inst.destn] = res;
	pc_increase(4);
}
void piplinecpu::AND(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	word res = r1 & r2;
	r[inst.destn] = res;
	pc_increase(4);
}
void piplinecpu::ANDI(const instruction& inst) {
	word r1 = rs;
	word r2 = inst.i_imi;
	word res = r1 & r2;
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::BEQ(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	if (r1 == r2) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		pc_increase(4);
	}
}

void piplinecpu::BGEZ(const instruction& inst) {
	s_word r1 = rs;
	if (r1 >= 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		pc_increase(4);
	}
}

void piplinecpu::BGEZAL(const instruction& inst) {
	s_word r1 = rs;
	r[31] = npc + 4;
	if (r1 >= 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		pc_increase(4);
	}
}
void piplinecpu::BGTZ(const instruction& inst) {
	s_word r1 = rs;
	if (r1 > 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		pc_increase(4);
	}
}
void piplinecpu::BLEZ(const instruction& inst) {
	s_word r1 = rs;
	if (r1 <= 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		pc_increase(4);
	}
}
void piplinecpu::BLTZ(const instruction& inst) {
	s_word r1 = rs;
	if (r1 < 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		pc_increase(4);
	}
}
void piplinecpu::BLTZAL(const instruction& inst) {
	s_word r1 = rs;
	r[31] = npc + 4; // same stuff
	if (r1 < 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		pc_increase(4);
	}
}
void piplinecpu::BNE(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	if (r1 != r2) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		pc_increase(4);
	}
}
void piplinecpu::DIV(const instruction& inst) {
	s_word r1 = rs;
	s_word r2 = rt;
	if (r2 != 0) {
		LO = (s_word)r1 / r2;
		HI = (s_word)r1 % r2;
	}
	pc_increase(4);
}
void piplinecpu::DIVU(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	if (r2 != 0) {
		LO = (word)r1 / r2;
		HI = (word)r1 % r2;
	}
	pc_increase(4);
}
void piplinecpu::J(const instruction& inst) {
	pc = npc;
	npc = (npc & 0xF0000000) | (inst.j_add << 2);
}

void piplinecpu::JALR(const instruction& inst) {
	r[inst.destn] = npc + 4;
	word adr = rs;
	pc = npc;
	npc = adr;
}

void piplinecpu::JAL(const instruction& inst) {
	r[31] = npc + 4;
	pc = npc;
	npc = (word)((pc & 0xF0000000) | (inst.j_add << 2));
}

void piplinecpu::JR(const instruction& inst) {
	word jump_address = rs;
	pc = npc;
	npc = jump_address;
}

void piplinecpu::LB(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word res = m.read_b(adr);
	if (res >= 0x80) res = 0xFFFFFF00 | res;
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::LBU(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word res = m.read_b(adr);
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::LH(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word res = m.read_h(adr);
	if (res >= 0x8000) res = 0xFFFF0000 | res;
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::LHU(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word res = m.read_h(adr);
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::LUI(const instruction& inst) {
	word data = inst.i_imi << 16;
	r[inst.src_t] = data;
	pc_increase(4);
}

void piplinecpu::LW(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	//mem
	word res = m.read_w(adr);
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::LWL(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	//word w_adr = (word) ((base + offset) - (base + offset) % 4);
	word w_adr = (base + offset) - (base + offset) % 4;

	word full_word = m.read_w(w_adr);
	word res = r[inst.src_t];
	int w_off = offset & 0x3;
	switch (w_off) {
	case 0x0: res = full_word; break;
	case 0x1: res = (res & 0x000000FF) | (full_word & 0x00FFFFFF) << 8;  break;
	case 0x2: res = (res & 0x0000FFFF) | (full_word & 0x0000FFFF) << 16; break;
	case 0x3: res = (res & 0x00FFFFFF) | (full_word & 0x000000FF) << 24; break;
	}
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::LWR(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	//word w_adr = (word) ((base + offset) - (base + offset) % 4);
	word w_adr = (base + offset) - (base + offset) % 4;

	word full_word = m.read_w(w_adr);
	word res = rt;
	int w_off = offset & 0x3;
	switch (w_off) {
	case 0x0: res = (res & 0xFFFFFF00) | full_word >> 24; break;
	case 0x1: res = (res & 0xFFFF0000) | full_word >> 16; break;
	case 0x2: res = (res & 0xFF000000) | full_word >> 8;  break;
	case 0x3: res = full_word; break;
	}
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::MFHI(const instruction& inst) {
	word data = HI;
	//wb
	r[inst.destn] = data;
	pc_increase(4);
}

void piplinecpu::MFLO(const instruction& inst) {
	word data = LO;
	//wb
	r[inst.destn] = data;
	pc_increase(4);
}

void piplinecpu::MTHI(const instruction& inst) {
	HI = rs;
	pc_increase(4);
}

void piplinecpu::MTLO(const instruction& inst) {
	LO = rs;
	pc_increase(4);
}
void piplinecpu::MULT(const instruction& inst) {
	s_word r1 = rs;
	s_word r2 = rt;
	int64_t res = static_cast<int64_t> (r1) * static_cast<int64_t> (r2);
	LO = static_cast<word> (res & 0x00000000FFFFFFFF);
	HI = static_cast<word> ((res & 0xFFFFFFFF00000000) >> 32);
	pc_increase(4);
}
void piplinecpu::MULTU(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	uint64_t res = static_cast<uint64_t> (r1) * static_cast<uint64_t> (r2);
	LO = static_cast<word> (res & 0x00000000FFFFFFFF);
	HI = static_cast<word> ((res & 0xFFFFFFFF00000000) >> 32);
	pc_increase(4);
}
void piplinecpu::OR(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	word res = r1 | r2;
	r[inst.destn] = res;
	pc_increase(4);
}
void piplinecpu::ORI(const instruction& inst) {
	word r1 = rs;
	word res = r1 | inst.i_imi;
	r[inst.src_t] = res;
	pc_increase(4);
}

void piplinecpu::SB(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word val = r[inst.src_t] & 0x000000FF;
	m.write_b(adr, val);
	pc_increase(4);
}

void piplinecpu::SH(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word val = rt & 0x0000FFFF;
	//mem
	m.write_h(adr, val);
	pc_increase(4);
}

void piplinecpu::SLL(const instruction& inst) {
	word r1 = rt;
	word res = r1 << inst.shamt;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}

void piplinecpu::SLLV(const instruction& inst) {
	word r1 = rs & 0x1F;
	word r2 = rt;
	word res = r2 << r1;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}

void piplinecpu::SLT(const instruction& inst) {
	s_word r1 = rs;
	s_word r2 = rt;
	word res = (r1 < r2) ? 1 : 0;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}
void piplinecpu::SLTI(const instruction& inst) {
	s_word r1 = rs;
	s_word r2 = sign_extend_imi(inst);
	word res = (r1 < r2) ? 1 : 0;
	//wb
	r[inst.src_t] = res;
	pc_increase(4);
}
void piplinecpu::SLTIU(const instruction& inst) {
	word r1 = rs;
	word r2 = sign_extend_imi(inst);
	word res = (r1 < r2) ? 1 : 0;
	//wb
	r[inst.src_t] = res;
	pc_increase(4);
}
void piplinecpu::SLTU(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	word res = (r1 < r2) ? 1 : 0;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}

void piplinecpu::SRA(const instruction& inst) {
	s_word r1 = rt;
	s_word res = r1 >> inst.shamt;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}

void piplinecpu::SRAV(const instruction& inst) {
	word r1 = rs;
	s_word r2 = rt;
	s_word res = r2 >> r1;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}

void piplinecpu::SRL(const instruction& inst) {
	word r1 = rt;
	word res = r1 >> inst.shamt;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}

void piplinecpu::SRLV(const instruction& inst) {
	word r1 = rs & 0x1F;
	word r2 = rt;
	word res = r2 >> r1;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}

void piplinecpu::SUB(const instruction& inst) {
	s_word r1 = rs;
	s_word r2 = rt;
	s_word res = r1 - r2;
	if ((r1 >= 0 && r2 < 0 && res < 0) || (r1 < 0 && r2 >= 0 && res > 0)) {
		std::cerr << "exception: arithmetic error" << std::endl;
		std::exit(-10);
	}
	else {
		//wb
		r[inst.destn] = res;
		pc_increase(4);
	}
}
void piplinecpu::SUBU(const instruction& inst) { // not tested
	word r1 = rs;
	word r2 = rt;
	word res = r1 - r2;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}
void piplinecpu::SW(const instruction& inst) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word val = rt;
	//mem
	m.write_w(adr, val);
	pc_increase(4);
}
void piplinecpu::XOR(const instruction& inst) {
	word r1 = rs;
	word r2 = rt;
	word res = r1 ^ r2;
	//wb
	r[inst.destn] = res;
	pc_increase(4);
}
void piplinecpu::XORI(const instruction& inst) {
	word r1 = rs;
	word res = r1 ^ inst.i_imi;
	//wb
	r[inst.src_t] = res;
	pc_increase(4);
}



