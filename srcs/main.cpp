#include <iostream>

#include "Simulation.hh"
#include "Robot.hh"

extern "C" {
#include "extApi.h"
}
using namespace std;

int main() {
  try {
    Simulation simulation = Simulation();
    return (simulation.run());
  }
  catch (const std::exception &e) {
    cerr << "error: " << e.what() << endl;
    return (-1);
  }
}
