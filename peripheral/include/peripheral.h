#ifndef peripheral_h
#define peripheral_h

#include <string>
#include <vector>

class Peripheral {
protected:
 std::vector<int16_t> memory_mapped_io;

 public:
  Peripheral(){};
  ~Peripheral(){};
  void ReadCallback();
  void WriteCallback();
};

#endif