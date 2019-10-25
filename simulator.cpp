#include <iostream>
#include <string>
#include "memory.hpp"
#include "cpu.hpp"
#include "piplinecpu.hpp"

int main(int argc, char const *argv[]) {
  std::string name_bin = "FIB.bin";

  //if(argc == 2) name_bin = argv[1];
  //else{
  //  std::cerr<<"binary not found"<<std::endl;
  //  exit(-21);
  //}

  //cpu c(name_bin);
  //c.run();

 /**/ piplinecpu c(name_bin);
  c.run5stage();

  return 0;
}
