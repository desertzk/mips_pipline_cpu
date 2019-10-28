#include "memory.hpp"
#include <fstream>
//debug
#include <iostream>
#include <cstdio>

using word = uint32_t;

memory::memory(){
  data = new std::vector<word>(0 , 1);
  inst = new std::vector<word>(0x1000000 , 0);
  n_inst = 0;
}

memory::memory(std::string binary){
  data = new std::vector<word>(0,0);
  inst = new std::vector<word>(0x1000000 / 4 , 0);

  std::ifstream infile;
  infile.open(binary, std::ios::binary);
  char buffer[4] = {0,0,0,0};
  int i = 0;

  if(infile.is_open()){
    while(!infile.eof()){
        infile.read(buffer, 4);
        infile.peek();
        word instruction = 0;
        instruction = (static_cast<uint8_t>(buffer[0]) << 24|
                       static_cast<uint8_t>(buffer[1]) << 16|
                       static_cast<uint8_t>(buffer[2]) <<  8|
                       static_cast<uint8_t>(buffer[3]));
        (*inst)[i] = instruction;
        i++;
        if(i > (0x1000000 / 4) ){
          std::cerr << "error: binary too long" << '\n';
          std::exit(-11);
        }
    }
  }else{
    std::cerr << "error: couldn't open binary" << '\n';
    std::exit(-21);
  }
  n_inst = i + 4;
  //std::cerr << "Number of instructions: " << i << '\n';
  infile.close();
}

memory::~memory(){
  delete inst;
  delete data;
}

void memory::write_w(word adr, word new_data){
	std::cout << "write from:" << adr<<" newdata:"<< new_data;
  if(adr >= 0x20000000 && adr < 0x24000000 && adr % 4 == 0){
    int index = (adr - 0x20000000) >> 2;
    if(index >= data->size())
      data->resize(index + 1, 0);

    (*data)[index] = new_data;
  }else if(adr == 0x30000004){
    // TODO: test
    std::putchar(new_data & 0xFF);

    if (std::ferror(stdout)) std::exit (-21);
  }else{
    std::cerr << "error: trying to write_word to address: " << adr << '\n';
    std::exit(-11);
  }
}

void memory::write_h(word adr, word new_data){
  if(adr >= 0x20000000 && adr < 0x24000000 && adr % 2 == 0){
    int index = (adr - 0x20000000) >> 2;
    if(index >= data->size())
      data->resize(index + 1, 0);

    word old_data = (*data)[index];
    word combined_data;

    switch(adr & 0x3){
      case 0x0: combined_data = ((old_data & 0x0000FFFF) | (new_data << 16));  break;
      case 0x2: combined_data = ((old_data & 0xFFFF0000) | new_data);
    }

    (*data)[index] = combined_data;
  }else if((adr >> 2) == (0x30000004 >> 2) && adr % 2 == 0){
    // TODO: test
    if(adr % 4 == 2) std::putchar(new_data & 0xFF);
    else std::putchar(0);

    if (std::ferror(stdout)) std::exit (-21);
  }else{
    std::cerr << "error: trying to write_half to address: " << adr << '\n';
    std::exit(-11);
  }
}

void memory::write_b(word adr, word new_data){
  if(adr >= 0x20000000 && adr < 0x24000000){
    int index = (adr - 0x20000000) >> 2;
    if(index >= data->size()){
      data->resize(index + 1, 0);
    }

    word old_data = (*data)[index];
    word combined_data;

    switch(adr & 0x3){
      case 0x0: combined_data = ((old_data & 0x00FFFFFF) | (new_data << 24)); break;
      case 0x1: combined_data = ((old_data & 0xFF00FFFF) | (new_data << 16)); break;
      case 0x2: combined_data = ((old_data & 0xFFFF00FF) | (new_data <<  8)); break;
      case 0x3: combined_data = ((old_data & 0xFFFFFF00) | new_data);
    }

    (*data)[index] = combined_data;
  }else if((adr >> 2) == (0x30000004 >> 2)){
    // TODO: test
    if(adr % 4 == 3) std::putchar(new_data & 0xFF);
    else std::putchar(0);

    if (std::ferror(stdout)) std::exit (-21);
  }else{
    std::cerr << "error: trying to write_byte to address: " << adr << '\n';
    std::exit(-11);
  }
}

word memory::read_w(word adr){
	std::cout << "read from:" << adr;
  if((adr >= 0x20000000 && adr < 0x24000000) && adr % 4 == 0){
    int index = (int) (adr - 0x20000000) / 4;
    if(index >= data->size()) return 0;
    else return (*data)[index];
  }
  if((adr >= 0x10000000 && adr < 0x11000000) && adr % 4 == 0){
    int index = (int) (adr - 0x10000000) / 4;
    return (*inst)[index];
  }
  else if(adr == 0x30000000){
    word in_w = std::getchar();
    if (std::ferror(stdin)) std::exit(-21);

    std::cerr << "read_w, in char: " << in_w << '\n';
    return (in_w == EOF) ? -1 : in_w & 0x000000FF;
  }
  std::cerr << "error: trying to read word from address: " << adr << '\n';
  std::exit(-11);
  return 0;
 }

word memory::read_h(word adr){
  word word_data;
  if((adr >= 0x20000000 && adr < 0x24000000) && adr % 2 == 0){
    int index = (int) (adr - 0x20000000) / 4;
    if(index >= data->size()) return 0;
    word_data = (*data)[index];
  }
  else if((adr >= 0x10000000 && adr < 0x11000000) && adr % 2 == 0){
    int index = (int) (adr - 0x10000000) / 4;
    word_data = (*inst)[index];
  }
  else if((adr >> 2) == (0x30000000 >> 2) && adr % 2 == 0){
    word in_w = std::getchar();
    if (std::ferror(stdin)) std::exit(-21);

    if(adr % 4 == 2){
      std::cerr << "read_h, in char: " << in_w << '\n';
      return (in_w == EOF) ? 0xFFFF : in_w & 0x00FF;
    }
    return (in_w == EOF) ? 0xFFFF : 0;
  }
  else{
    std::cerr << "error: trying to read halfword from address: " << adr << '\n';
    std::exit(-11);
  }

  switch(adr & 0x2){
    case 0x0: return (word_data & 0xFFFF0000) >> 16; break;
    case 0x2: return (word_data & 0x0000FFFF);
  }
  return 0;
}

word memory::read_b(word adr){
  word word_data;
  if(adr >= 0x20000000 && adr < 0x24000000){
    int index = (int) (adr - 0x20000000) / 4;
    if(index >= data->size()) return 0;
    word_data = (*data)[index];
  }
  else if(adr >= 0x10000000 && adr < 0x11000000){
    int index = (int) (adr - 0x10000000) / 4;
    word_data = (*inst)[index];
  }
  else if((adr >> 2) == (0x30000000 >> 2)){  //shoudln't be used: think about the outcome
    word in_w = std::getchar();
    if (std::ferror(stdin)) std::exit(-21);

    if(adr % 4 == 3){
      std::cerr << "read_b, in char: " << in_w << '\n';
      return (in_w == EOF) ? 0xFF : in_w & 0xFF;
    }
    return (in_w == EOF) ? 0xFF : 0;
  }
  else{
    std::cerr << "error: trying to read byte from address: " << adr << '\n';
    std::exit(-11);
  }

  switch(adr & 0x3){
    case 0x0: return (word_data & 0xFF000000) >> 24;  break;
    case 0x1: return (word_data & 0x00FF0000) >> 16;  break;
    case 0x2: return (word_data & 0x0000FF00) >> 8;   break;
    case 0x3: return (word_data & 0x000000FF);
  }
}

//word memory::read_inst(int adr){
//  if((adr >= 0x10000000 && adr < 0x11000000) && adr % 4 == 0){
//    int index = (adr - 0x10000000) / 4;
//    return (*inst)[index];
//  }
//  std::cerr << "error: trying to read instruction from address: " << adr << '\n';
//  std::exit(-11);
//}

word memory::read_inst(int adr) {
	if ((adr >= 0x10000000 && adr < 0x11000000) && adr % 4 == 0) {
		int index = (adr - 0x10000000) / 4;
		return (*inst)[index];
	}
	else {
		return -1;
	}

}

void memory::print_mem() const{
  std::cerr << "Instructions:" << '\n';
  for(int i = 0; i < inst->size(); i++)
    if((*inst)[i]!= 0)
      std::cerr << i << "\t" << (*inst)[i] << '\n';
  std::cerr << "Data:" << '\n';
  for(int i = 0; i < data->size(); i++)
    std::cerr << i << "\t" << (*data)[i] << '\n';
  std::cerr<<std::endl;
}
