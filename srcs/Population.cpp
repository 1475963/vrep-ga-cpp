#include "Population.hh"

Population::Population() {
  this->initialize(_defaultMaxPop);
}

Population::Population(uint16_t maxPop) {
  this->initialize(maxPop);
}

void  Population::initialize(uint16_t maxPop) {
  RandomGenerator &rg = RandomGenerator::getInstance();

  _population.clear();

  for (uint16_t i = 0; i < maxPop; i++) {
    _population.push_back(new Individual(rg.i_between(1, 10)));
  }
}

fitness_t       Population::evaluateBatch() {
  fitness_t     globalFitness = 0;

  for (Individual *individual: _population) {
    individual->evaluate();
    globalFitness += individual->getScore();
  }

  return (globalFitness / (_population.size() > 0 ? _population.size() : 1));
}

void            Population::termDisplay() const {
  for (Individual *individual : _population) {
    individual->termDisplay();
  }
}

population_t    Population::getPopulation() {
  return (_population);
}

void            Population::addChild(Individual *individual) {
  _population.push_back(individual);
}
