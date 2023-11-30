#ifndef clock_h
#define clock_h

#include <cstdint>
#include <map>

#include "peripheral.h"

typedef std::pair<uint16_t, uint8_t> F_DCO;
typedef std::map<F_DCO, uint32_t> FrequencyMap;

union DCOControlUnion {
  struct {
    uint8_t MODx : 5;
    uint8_t DCOx : 3;
  };
  uint8_t val;
};

union BCSCTL1_Union {
  struct {
    uint8_t RSELx : 4;
    uint8_t DIVAx : 2;
    uint8_t XTS : 1;
    uint8_t XT2OFF : 1;
  };
  uint8_t val;
};

union BCSCTL2_Union {
  struct {
    uint8_t DCOR : 1;
    uint8_t DIVSx : 2;
    uint8_t SELS : 1;
    uint8_t DIVMx : 2;
    uint8_t SELMx : 2;
  };
  uint8_t val;
};

union BCSCTL3_Union {
  struct {
    uint8_t LFXT1OF : 1;
    uint8_t XT2OF : 1;
    uint8_t XCAPx : 2;
    uint8_t LFXT1Sz : 2;
    uint8_t XT2Sx : 2;
  };
  uint8_t val;
};

class Clock : public Peripheral {
 public:
  Clock();
  ~Clock(){};

  F_DCO MakePair(int a, int b);
  uint32_t MHZ(double val);
  uint32_t MHZ(int val);
  uint32_t GetDCO();

  FrequencyMap frequency_map;
  DCOControlUnion DCO;
  BCSCTL1_Union BCSCTL1;
  BCSCTL2_Union BCSCTL2;
  BCSCTL3_Union BCSCTL3;
};

#endif