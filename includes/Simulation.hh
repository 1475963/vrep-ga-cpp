#pragma once

#include <ctime>
#include <vector>
#include <string>
#include "Logger.hh"
#include "VrepConnection.hh"
#include "types.hh"
#include "Population.hh"

class Simulation {

public:

  Simulation();
  int run();

private:
  const static uint16_t _maxPop = 2;
  const static uint16_t _maxTries = 2;
  const static uint16_t _maxRobots = 4;

  Population            _population;
  simxInt               _clientID;
  std::vector<Robot>    _robots;
  Logger		_logger;

  void breedingSeason();
  couple_t makeCouple(fitness_t fitness);
  couple_t crossOverSinglePoint(const Individual& i1, const Individual &i2);
  void logPopulation(int index);
};
