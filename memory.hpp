#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <string>
#include <vector>

using word = uint32_t;

class memory{
public:
  std::vector<word>* inst;
  std::vector<word>* data;
  int n_inst;
public:
  memory();
  memory(std::string binary);
  ~memory();
  void write_w(word adr, word new_data);
  void write_h(word adr, word new_data);
  void write_b(word adr, word new_data);
  word read_w(word adr);
  word read_h(word adr);
  word read_b(word adr);
  word read_inst(int adr);

  void print_mem() const;
};

#endif
