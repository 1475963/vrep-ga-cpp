#pragma once

#include <ctime>
#include <vector>
#include <string>
#include <map>
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
  couple_t makeCouple(fitness_t);
  void logPopulation(int index);

  couple_t crossOverSinglePoint(const Individual&, const Individual &);
  couple_t crossOverCutAndSplice(const Individual &, const Individual &);
  couple_t crossOverTwoPoint(const Individual &, const Individual &);
  couple_t crossOverUniform(const Individual &, const Individual &);
  
  typedef couple_t (Simulation::*func_ptr_t)(const Individual &, const Individual &);
  static const std::map<const std::string, const func_ptr_t> crossovers;
};
