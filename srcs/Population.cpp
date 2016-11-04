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

  for (Individual *individual : _population) {
    individual->evaluate();
    globalFitness += individual->getScore();
  }

  return (globalFitness / (_population.size() > 0 ? _population.size() : 1));
}

void            Population::mutateBatch() {
  for (Individual *individual : _population) {
    individual->mutate();
  }
}

Individual      *Population::getElite() {
  Individual    *best = nullptr;

  for (uint16_t i = 0; i < _population.size(); ++i) {
    if (!best || _population[i]->getScore() > best->getScore()) {
      best = _population[i];
    }
  }

  return best;
}

Individual      *Population::getWorst() {
  Individual    *worst = nullptr;

  for (uint16_t i = 0; i < _population.size(); ++i) {
    if (!worst || _population[i]->getScore() < worst->getScore()) {
      worst = _population[i];
    }
  }

  return worst;
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
