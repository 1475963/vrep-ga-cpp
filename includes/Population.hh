#pragma once

#include <iostream>
#include <sstream>
#include "Individual.hh"
#include "RandomGenerator.hpp"

typedef std::vector<Individual> population_t;

class Population {

public:
    Population();
    Population(uint16_t maxPop);
    population_t    &initialize(uint16_t maxPop);
    fitness_t       evaluateBatch();
    void            termDisplay();
    population_t    getPopulation() const;

private:
    static const uint16_t   _defaultMaxPop = 50;
    population_t            _population;
};