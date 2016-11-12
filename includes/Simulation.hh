#pragma once

#include <ctime>
#include <vector>
#include <string>
#include <omp.h>
#include <cmath>
#include <map>
#include <algorithm>

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
  using fp = couple_t (Simulation::*)() const;
  static const std::map<const std::string, const fp> selections;

  Population            _population;
  simxInt               _clientID;
  std::vector<Robot>    _robots;
  Logger		_logger;

  const std::string	_globalLogFile = "global.log";
  const uint16_t	_maxRobots;
  const uint16_t	_maxPop;
  const uint16_t	_maxGenerations;

  couple_t		fitnessProportionalSelection() const;
  couple_t		tournamentSelection() const;
  couple_t		crossOverSinglePoint(const Individual& i1, const Individual &i2);
  void			logPopulation(int index);
  void			logStats(const Individual &i1, const Individual &i2, double avgFit);
};
