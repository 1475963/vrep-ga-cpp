#pragma once

#include <ctime>
#include <vector>
#include <string>
#include <omp.h>
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
  std::vector<Robot>    _robots;

  const uint16_t _maxRobots;
  const uint16_t _maxPop;
  const uint16_t _maxGenerations;

  void breedingSeason();
  couple_t makeCouple(fitness_t fitness);
  couple_t crossOverSinglePoint(const Individual& i1, const Individual &i2);
};
