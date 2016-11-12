#include <iostream>

#include "Simulation.hh"
#include "Robot.hh"

extern "C" {
#include "extApi.h"
}
using namespace std;

int main() {
  try {
    Simulation simulation;
    return (simulation.run());
  }
  catch (const char *e) {
    cerr << "error: " << e << endl;
    return (-1);
  }
}
