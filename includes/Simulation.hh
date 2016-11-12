#pragma once

#include <ctime>
#include <vector>
#include <string>
#include <omp.h>
#include <cmath>

#include "Logger.hh"
#include "VrepConnection.hh"
#include "types.hh"
#include "Population.hh"

class Simulation {

public:

  Simulation();
  ~Simulation();
  int run();

private:
  Population            _population;
  simxInt               _clientID;
  std::vector<Robot>    _robots;
  Logger		_logger;
  const std::string	_globalLogFile = "global.log";

  const uint16_t _maxRobots;
  const uint16_t _maxPop;
  const uint16_t _maxGenerations;

  void breedingSeason();
  couple_t makeCouple(fitness_t fitness);
  couple_t crossOverSinglePoint(const Individual& i1, const Individual &i2);
  void logPopulation(int index);
};
