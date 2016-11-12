#pragma once

#include <iostream>
#include <sstream>
#include "Individual.hh"
#include "RandomGenerator.hpp"

class Population {

public:
  Population() = default;
  Population(uint16_t maxPop);
  void		initialize(uint16_t maxPop);
  fitness_t	evaluateBatch();
  void		mutateBatch();
  Individual	&getElite();
  Individual	&getWorst();
  void		termDisplay() const;
  void		addIndividual(const Individual &individual);
  uint		size() const;
  Individual        &operator[](uint index);
  const Individual  &at(uint index) const;


private:
    population_t            _population;
};
