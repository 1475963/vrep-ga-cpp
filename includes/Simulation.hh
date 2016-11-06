#pragma once

#include <ctime>
#include <vector>
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
  std::vector<Robot>  _robots;
//  const Robot           _robot;
  const static uint16_t _maxPop = 5;
  const static uint16_t _maxTries = 20;
  const static uint16_t _maxRobots = 4;

  void breedingSeason();
  couple_t makeCouple(fitness_t fitness);
  couple_t crossOverSinglePoint(const Individual& i1, const Individual &i2);
};
