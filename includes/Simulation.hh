#pragma once

#include "VrepConnection.hh"
#include "types.hh"
#include "Individual.hh"
#include "RandomGenerator.hpp"

class Simulation {
  //VrepConnection _connection;

public:

  Simulation();
  ~Simulation();
  void run();

private:

  Population *generatePopulation(uint16_t);
  double evaluateIndividual(Individual);
  double evaluate(Population);

};
