#include"piplinecpu.hpp"

#include <string>
#include <iostream> //debug
#include <stdexcept> //std::exit
#include <condition_variable>
#include "instruction.hpp"


using namespace std;

piplinecpu::piplinecpu(): cpu(),rs(0),rt(0), is_finish(false){
	pc = 0x10000000;
	npc = 0x10000004;
	LO = 0;
	HI = 0;
}

piplinecpu::piplinecpu(std::string binary) :cpu(binary), rs(0), rt(0), is_finish(false) {
	pc = 0x10000000;
	npc = 0x10000004;
	LO = 0;
	HI = 0;
}



void piplinecpu::run5stage() {
	is_ex = true;
	thread trdcycle(&piplinecpu::CycleControl, this); // 
	thread trdif(&piplinecpu::IF, this); // 
	thread trdid(&piplinecpu::ID, this); // 
	thread trdex(&piplinecpu::EX, this); // 
	
	//thread trdex(&piplinecpu::EX, this); // 
	trdcycle.join();
	trdif.join();
	trdid.join(); 
	trdex.join();
	//thread trdif(&piplinecpu::IF, this); // 
	/*while (true) {
		word instruct = this->IF();
		instruction c_inst = this->ID(instruct);
		this->EX(c_inst);

		if (pc == 0) {
			exit(r[2]);
		}
	}*/
}


void piplinecpu::CycleControl()
{
	while (!is_finish)
	{
		std::this_thread::sleep_for(500ms);
		unique_lock<std::mutex> lk(mutcycle);
		++cycle;
		cout << "===============================" << cycle << "=================================" << endl;
		cvcycle.notify_all();
		//std::this_thread::sleep_for(3s);
		
	}

	//return instruction;
}

void piplinecpu::IF()
{
	while (pc!=-1)
	{
		unique_lock<std::mutex> lk(mutcycle);
		cvcycle.wait(lk, [this] {return cycle> ifcycle; });
		
		word instruction = m.read_inst(pc);
		if (instruction == -1/*|| instruction == 0*/)
		{
			is_finish = true;
			return;
		}
		
		cout <<"IF:"<< instruction <<"pc:"<<pc<< endl;
		pc_increase(4);
		std::lock_guard<std::mutex> lock(mutif);
		queue_if.push(instruction);
		++ifcycle;
		cvcycle.notify_one();
	}

	//return instruction;
}

void piplinecpu::ID()
{
	while (!is_finish || queue_if.size()>0)
	{

		unique_lock<std::mutex> lk(mutcycle);
		cvcycle.wait(lk, [this] {return queue_if.size()>0; });
		
		word next_instruction = queue_if.front();
		queue_if.pop();
		//mutif.unlock();

		instruction inst(next_instruction);
		s_word rs = r[inst.src_s];
		s_word rt = r[inst.src_t];
		cout << "Id:" << inst << endl;
		std::lock_guard<std::mutex> lock(mutid);
		queue_id.push(make_tuple(inst, rs,rt));
		cvcycle.notify_one();
	}
	
}

void piplinecpu::EX() {
	while (!is_finish || queue_id.size() > 0)
	{
		unique_lock<std::mutex> lk(mutcycle);
		cvcycle.wait(lk, [this] {return queue_id.size() > 0; });
		instruction inst = get<0>(queue_id.front());
		s_word rs= get<1>(queue_id.front());
		s_word rt= get<2>(queue_id.front());
		queue_id.pop();
		//mutid.unlock();
		cout <<"EX:" << inst<<endl;
		switch (inst.type) {
			case 'r': execute_r(inst, rs, rt); break;
			case 'i': execute_i(inst, rs, rt); break;
			case 'j': execute_j(inst, rs, rt); break;
			default: 
				std::cerr << "error: incorect instruction type"<<endl; //std::exit(-20);
		

		}

		std::lock_guard<std::mutex> lock(mutex);
		
		queue_ex.push(inst);
		//cvex.notify_one();
		cvcycle.notify_one();
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
void piplinecpu::ADD(const instruction& inst, s_word rs, s_word rt) {
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
	//pc_increase(4);
}
void piplinecpu::ADDI(const instruction& inst, s_word rs, s_word rt) {
	//TODO: check immiatde sing extension
	s_word r1 = rs;
	s_word imi = sign_extend_imi(inst);
	s_word res = r1 + imi;

	if ((res <= 0 && r1 > 0 && imi > 0) || (res >= 0 && r1 < 0 && imi < 0)) {
		std::cerr << "exception: arithmetic error" << '\n';
		std::exit(-10);
	}

	r[inst.src_t] = res;
	//pc_increase(4);
}
void piplinecpu::ADDIU(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word imi = sign_extend_imi(inst);
	s_word res = r1 + imi;
	r[inst.src_t] = res;
	//pc_increase(4);
}
void piplinecpu::ADDU(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	word res = r1 + r2;
	r[inst.destn] = res;
	//pc_increase(4);
}
void piplinecpu::AND(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	word res = r1 & r2;
	r[inst.destn] = res;
	//pc_increase(4);
}
void piplinecpu::ANDI(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = inst.i_imi;
	word res = r1 & r2;
	r[inst.src_t] = res;
	//pc_increase(4);
}

void piplinecpu::BEQ(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	if (r1 == r2) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		//pc_increase(4);
	}
}

void piplinecpu::BGEZ(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	if (r1 >= 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		//pc_increase(4);
	}
}

void piplinecpu::BGEZAL(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	r[31] = npc + 4;
	if (r1 >= 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		//pc_increase(4);
	}
}
void piplinecpu::BGTZ(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	if (r1 > 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		//pc_increase(4);
	}
}
void piplinecpu::BLEZ(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	if (r1 <= 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		//pc_increase(4);
	}
}
void piplinecpu::BLTZ(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	if (r1 < 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		//pc_increase(4);
	}
}
void piplinecpu::BLTZAL(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	r[31] = npc + 4; // same stuff
	if (r1 < 0) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		//pc_increase(4);
	}
}
void piplinecpu::BNE(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	if (r1 != r2) {
		word offset = sign_extend_imi(inst) << 2;
		pc_increase(offset);
	}
	else {
		//pc_increase(4);
	}
}
void piplinecpu::DIV(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	s_word r2 = rt;
	if (r2 != 0) {
		LO = (s_word)r1 / r2;
		HI = (s_word)r1 % r2;
	}
	//pc_increase(4);
}
void piplinecpu::DIVU(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	if (r2 != 0) {
		LO = (word)r1 / r2;
		HI = (word)r1 % r2;
	}
	//pc_increase(4);
}
void piplinecpu::J(const instruction& inst, s_word rs, s_word rt) {
	pc = npc;
	npc = (npc & 0xF0000000) | (inst.j_add << 2);
}

void piplinecpu::JALR(const instruction& inst, s_word rs, s_word rt) {
	r[inst.destn] = npc + 4;
	word adr = rs;
	pc = npc;
	npc = adr;
}

void piplinecpu::JAL(const instruction& inst, s_word rs, s_word rt) {
	r[31] = npc + 4;
	pc = npc;
	npc = (word)((pc & 0xF0000000) | (inst.j_add << 2));
}

void piplinecpu::JR(const instruction& inst, s_word rs, s_word rt) {
	word jump_address = rs;
	pc = npc;
	npc = jump_address;
}

/*void piplinecpu::LB(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	
	//word res = m.read_b(adr);
	queue_mem.push(make_tuple("rb", adr, 0));
	if (res >= 0x80) res = 0xFFFFFF00 | res;
	//wb
	r[inst.src_t] = res;
	//pc_increase(4);
}

void piplinecpu::LBU(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	//word res = m.read_b(adr);
	queue_mem.push(make_tuple("rb", adr, 0));
	r[inst.src_t] = res;
	//pc_increase(4);
}*/

void piplinecpu::LH(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	//word res = m.read_h(adr);
	queue_mem.push(make_tuple("rh", adr, 0));
	//if (res >= 0x8000) res = 0xFFFF0000 | res;
	//wb
	//r[inst.src_t] = res;
	//pc_increase(4);
}

void piplinecpu::LH_mem(word res) {
	

	//if (res >= 0x8000) res = 0xFFFF0000 | res;
	queue_wb.push(res);
	//wb
	//r[inst.src_t] = res;
	//pc_increase(4);
}

void piplinecpu::LH_wb(int src_t,word res) {


	if (res >= 0x8000) res = 0xFFFF0000 | res;
	//wb
	r[src_t] = res;
	//pc_increase(4);
}

/*
void piplinecpu::LHU(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	//word res = m.read_h(adr);
	queue_mem.push(make_tuple("rh", adr, 0));
	//wb
	r[inst.src_t] = res;
	//pc_increase(4);
}
*/
void piplinecpu::LUI(const instruction& inst, s_word rs, s_word rt) {
	word data = inst.i_imi << 16;
	//wb
	r[inst.src_t] = data;
	//pc_increase(4);
}
/*
void piplinecpu::LW(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	//mem
	//word res = m.read_w(adr);
	queue_mem.push(make_tuple("rw", adr,0));
	//wb
	r[inst.src_t] = res;
	//pc_increase(4);
}

void piplinecpu::LWL(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	//word w_adr = (word) ((base + offset) - (base + offset) % 4);
	word w_adr = (base + offset) - (base + offset) % 4;

	//word full_word = m.read_w(w_adr);
	queue_mem.push(make_tuple("rw", w_adr, 0));
	word res = r[inst.src_t];
	int w_off = offset & 0x3;
	switch (w_off) {
	case 0x0: res = full_word; break;
	case 0x1: res = (res & 0x000000FF) | (full_word & 0x00FFFFFF) << 8;  break;
	case 0x2: res = (res & 0x0000FFFF) | (full_word & 0x0000FFFF) << 16; break;
	case 0x3: res = (res & 0x00FFFFFF) | (full_word & 0x000000FF) << 24; break;
	}
	//wb
	r[inst.src_t] = res;
	//pc_increase(4);
}

void piplinecpu::LWR(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	//word w_adr = (word) ((base + offset) - (base + offset) % 4);
	word w_adr = (base + offset) - (base + offset) % 4;

	//word full_word = m.read_w(w_adr);
	queue_mem.push(make_tuple("rw", w_adr, 0));
	word res = rt;
	int w_off = offset & 0x3;
	switch (w_off) {
	case 0x0: res = (res & 0xFFFFFF00) | full_word >> 24; break;
	case 0x1: res = (res & 0xFFFF0000) | full_word >> 16; break;
	case 0x2: res = (res & 0xFF000000) | full_word >> 8;  break;
	case 0x3: res = full_word; break;
	}
	//wb
	r[inst.src_t] = res;
	//pc_increase(4);
}
*/
void piplinecpu::MFHI(const instruction& inst, s_word rs, s_word rt) {
	word data = HI;
	//wb
	r[inst.destn] = data;
	//pc_increase(4);
}

void piplinecpu::MFLO(const instruction& inst, s_word rs, s_word rt) {
	word data = LO;
	//wb
	r[inst.destn] = data;
	//pc_increase(4);
}

void piplinecpu::MTHI(const instruction& inst, s_word rs, s_word rt) {
	HI = rs;
	//pc_increase(4);
}

void piplinecpu::MTLO(const instruction& inst, s_word rs, s_word rt) {
	LO = rs;
	//pc_increase(4);
}
void piplinecpu::MULT(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	s_word r2 = rt;
	int64_t res = static_cast<int64_t> (r1) * static_cast<int64_t> (r2);
	LO = static_cast<word> (res & 0x00000000FFFFFFFF);
	HI = static_cast<word> ((res & 0xFFFFFFFF00000000) >> 32);
	//pc_increase(4);
}
void piplinecpu::MULTU(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	uint64_t res = static_cast<uint64_t> (r1) * static_cast<uint64_t> (r2);
	LO = static_cast<word> (res & 0x00000000FFFFFFFF);
	HI = static_cast<word> ((res & 0xFFFFFFFF00000000) >> 32);
	//pc_increase(4);
}
void piplinecpu::OR(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	word res = r1 | r2;
	r[inst.destn] = res;
	//pc_increase(4);
}
void piplinecpu::ORI(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word res = r1 | inst.i_imi;
	r[inst.src_t] = res;
	//pc_increase(4);
}

void piplinecpu::SB(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word val = r[inst.src_t] & 0x000000FF;
	
	//m.write_b(adr, val);
	queue_mem.push(make_tuple("wb",adr, val));
	//pc_increase(4);
}

void piplinecpu::SH(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word val = rt & 0x0000FFFF;
	//mem
	//m.write_h(adr, val);
	queue_mem.push(make_tuple("wh",adr, val));
}

void piplinecpu::SLL(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rt;
	word res = r1 << inst.shamt;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}

void piplinecpu::SLLV(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs & 0x1F;
	word r2 = rt;
	word res = r2 << r1;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}

void piplinecpu::SLT(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	s_word r2 = rt;
	word res = (r1 < r2) ? 1 : 0;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}
void piplinecpu::SLTI(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rs;
	s_word r2 = sign_extend_imi(inst);
	word res = (r1 < r2) ? 1 : 0;
	//wb
	r[inst.src_t] = res;
	//pc_increase(4);
}
void piplinecpu::SLTIU(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = sign_extend_imi(inst);
	word res = (r1 < r2) ? 1 : 0;
	//wb
	r[inst.src_t] = res;
	//pc_increase(4);
}
void piplinecpu::SLTU(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	word res = (r1 < r2) ? 1 : 0;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}

void piplinecpu::SRA(const instruction& inst, s_word rs, s_word rt) {
	s_word r1 = rt;
	s_word res = r1 >> inst.shamt;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}

void piplinecpu::SRAV(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	s_word r2 = rt;
	s_word res = r2 >> r1;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}

void piplinecpu::SRL(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rt;
	word res = r1 >> inst.shamt;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}

void piplinecpu::SRLV(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs & 0x1F;
	word r2 = rt;
	word res = r2 >> r1;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}

void piplinecpu::SUB(const instruction& inst, s_word rs, s_word rt) {
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
		//pc_increase(4);
	}
}
void piplinecpu::SUBU(const instruction& inst, s_word rs, s_word rt) { // not tested
	word r1 = rs;
	word r2 = rt;
	word res = r1 - r2;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}
void piplinecpu::SW(const instruction& inst, s_word rs, s_word rt) {
	s_word base = rs;
	s_word offset = sign_extend_imi(inst);
	word adr = base + offset;
	word val = rt;
	//mem
	//m.write_w(adr, val);
	queue_mem.push(make_tuple("ww",adr, val));
	//pc_increase(4);
}

void piplinecpu::XOR(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word r2 = rt;
	word res = r1 ^ r2;
	//wb
	r[inst.destn] = res;
	//pc_increase(4);
}

void piplinecpu::XORI(const instruction& inst, s_word rs, s_word rt) {
	word r1 = rs;
	word res = r1 ^ inst.i_imi;
	//wb
	r[inst.src_t] = res;
	//pc_increase(4);
}



