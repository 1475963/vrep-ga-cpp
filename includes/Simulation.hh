#pragma once

#include <ctime>
#include "VrepConnection.hh"
#include "types.hh"
#include "Population.hh"

class Simulation {

public:

  Simulation();
  int run();

private:
  Population            _population;
  simxInt               _clientID;
  const Robot           _robot;
  const static uint16_t _maxPop = 5;
  const static uint16_t _maxTries = 20;

  void breedingSeason();
  couple_t makeCouple(fitness_t fitness);
  couple_t crossOverSinglePoint(const Individual& i1, const Individual &i2);
};
