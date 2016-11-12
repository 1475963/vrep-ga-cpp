#pragma once

#include <ctime>
#include <vector>
#include <string>
#include <map>
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
  const static uint16_t _maxPop = 40;
  const static uint16_t _maxTries = 20;
  const static uint16_t _maxRobots = 4;

  void breedingSeason();
  couple_t makeCouple(fitness_t);

  couple_t crossOverSinglePoint(const Individual &, const Individual &);
  couple_t crossOverTwoPoint(const Individual &, const Individual &);

  typedef couple_t (Simulation::*func_ptr_t)(const Individual &, const Individual &);
  static const std::map<const std::string, const func_ptr_t> crossovers;
};
