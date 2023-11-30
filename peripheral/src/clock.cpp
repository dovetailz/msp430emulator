#include "clock.h"

#include <cmath>

uint32_t Clock::MHZ(double val) {
  return static_cast<uint32_t>(val * (std::pow(10, 6)));
}

uint32_t Clock::MHZ(int val) {
  return static_cast<uint32_t>(val * (std::pow(10, 6)));
}

F_DCO Clock::MakePair(int a, int b) {
  return std::make_pair<uint16_t, uint8_t>(static_cast<uint16_t>(a),
                                           static_cast<uint8_t>(b));
}

Clock::Clock() {
  frequency_map.emplace(MakePair(0, 0), MHZ(0.06));
  frequency_map.emplace(MakePair(0, 3), MHZ(0.12));
  frequency_map.emplace(MakePair(1, 3), MHZ(0.15));
  frequency_map.emplace(MakePair(2, 3), MHZ(0.21));
  frequency_map.emplace(MakePair(3, 3), MHZ(0.30));
  frequency_map.emplace(MakePair(4, 3), MHZ(0.41));
  frequency_map.emplace(MakePair(5, 3), MHZ(0.58));
  frequency_map.emplace(MakePair(6, 3), MHZ(0.80));
  frequency_map.emplace(MakePair(7, 3), MHZ(1.2));
  frequency_map.emplace(MakePair(8, 3), MHZ(1.6));
  frequency_map.emplace(MakePair(9, 3), MHZ(2.3));
  frequency_map.emplace(MakePair(10, 3), MHZ(3.4));
  frequency_map.emplace(MakePair(11, 3), MHZ(4.25));
  frequency_map.emplace(MakePair(12, 3), MHZ(6.0));
  frequency_map.emplace(MakePair(13, 3), MHZ(7.8));
  frequency_map.emplace(MakePair(14, 3), MHZ(12));
  frequency_map.emplace(MakePair(15, 3), MHZ(15.25));
  frequency_map.emplace(MakePair(15, 7), MHZ(21));
}