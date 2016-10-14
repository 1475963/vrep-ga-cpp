#include <iostream>

#include "Simulation.hh"
#include "Robot.hh"

extern "C" {
#include "extApi.h"
}
using namespace std;

int main() {
  auto simulation = Simulation();
  simulation.run();
  return 0;
}
