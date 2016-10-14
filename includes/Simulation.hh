#pragma once

#include <ctime>
#include "VrepConnection.hh"
#include "types.hh"
#include "Population.hh"

const uint16_t maxPop = 200;
const uint16_t maxTries = 200;

class Simulation {
  //VrepConnection _connection;

public:

  Simulation();
  ~Simulation();
  int run();

private:
  Population  _population;

};
