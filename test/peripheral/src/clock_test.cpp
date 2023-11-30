#include "clock_test.h"

TEST_F(ClockTest, MHZ) {
  // Test Int
  uint32_t val = 21000000;
  ASSERT_EQ(val, clock.MHZ(21)) << "MHZ incorrect";

  // Test float
  val = 210000;
  ASSERT_EQ(val, clock.MHZ(0.21)) << "MHZ incorrect";
}

TEST_F(ClockTest, MakePair) {
  auto pair = clock.MakePair(0, 3);
  EXPECT_EQ(pair.first, 0) << "Make pair first failed";
  EXPECT_EQ(pair.second, 3) << "Make pair second failed";
}

TEST_F(ClockTest, GetFrequency) {
  clock.DCO.val = 0xB0;
  clock.BCSCTL1.val = 0x86;

  EXPECT_EQ(clock.DCO.DCOx, 5);
  EXPECT_EQ(clock.DCO.MODx, 16);
  EXPECT_EQ(clock.BCSCTL1.RSELx, 6);

  auto fdco = static_cast<double>(clock.frequency_map.at(
      clock.MakePair(static_cast<int>(clock.BCSCTL1.RSELx), 3)));
  auto mod = static_cast<double>(clock.DCO.MODx);
  double mul = 32;

  std::cout << "FDCO: " << fdco << std::endl;
  std::cout << "MOD: " << mod << std::endl;

  auto frequency = (mul * fdco * fdco) / ((mod * fdco) + ((mul - mod) * fdco));
  std::cout << "Frequency: " << frequency << std::endl;
}