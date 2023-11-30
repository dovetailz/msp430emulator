#include "emulator.h"

void Emulator::Cycle() { this->debug.Step(); }

Emulator::Emulator(std::string filepath) {
  this->filepath = filepath;
  this->debug.LoadMem(this->filepath);
}

int main() {
  try {
    Emulator emulator(
        "/home/dovetailz/workspace_v12/Blinker/Debug/Blinker.out");

    while (true) {
      emulator.Cycle();
    }
  } catch (std::exception& e) {
    // Handle standard exceptions
    std::cerr << "Standard exception: " << e.what() << std::endl;
  }

  return 0;
}