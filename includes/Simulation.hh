#pragma once

#include <ctime>
#include "VrepConnection.hh"
#include "types.hh"
#include "Population.hh"

class Simulation {

public:

  Simulation();
  ~Simulation();
  int run();

private:
  Population  _population;
  simxInt     _clientID;
  const static uint16_t _maxPop = 200;
  const static uint16_t _maxTries = 200;

};
