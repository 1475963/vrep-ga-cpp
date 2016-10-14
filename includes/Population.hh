#pragma once

#include <iostream>
#include <sstream>
#include "Individual.hh"
#include "RandomGenerator.hpp"

class Population {

public:
    Population();
    Population(uint16_t);
    population_t    &initialize(uint16_t);
    fitness_t       evaluateBatch();
    void            termDisplay();
    population_t    &getPopulation();

private:
    const uint16_t  _defaultMaxPop = 50;
    population_t    _population;
};