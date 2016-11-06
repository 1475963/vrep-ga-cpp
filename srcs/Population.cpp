#include "Population.hh"

/*
** Population constructor generating a population of length maxPop
** and initialize each individual
**
** @param maxPop: population length, number of individuals in population
*/
Population::Population(uint16_t maxPop) {
  initialize(maxPop);
}

/*
** Initialize population, each individual is initialized with a random length
**
** @param maxPop: population length, number of individuals in population
*/
void  Population::initialize(uint16_t maxPop) {
  RandomGenerator &rg = RandomGenerator::getInstance();

  _population.clear();
  _population.resize(maxPop);

  for (uint16_t i = 0; i < maxPop; i++) {
    _population[i].initialize(rg.i_between(1, 10));
  }
}

/*
** Walk through the population to compute each individual's fitness score
**
** @return The average fitness in population
*/
fitness_t       Population::evaluateBatch() {
  fitness_t     globalFitness = 0;

  for (auto &individual : _population) {
    individual.evaluate();
    globalFitness += individual.getScore();
  }

  return (globalFitness / (_population.size() > 0 ? _population.size() : 1));
}

/*
** Walk through the population to eventually mutate an individual DNA
*/
void            Population::mutateBatch() {
  for (auto &individual : _population) {
    individual.mutate();
  }
}

/*
** Retrieve the individual with the best fitness score in the population
** should be changed in order to sort the population and return n first elements
**
** @return best individual
*/
Individual      &Population::getElite() {
  fitness_t	best = _population[0].getScore();
  uint16_t	bestId = 0;

  for (uint16_t i = 0; i < _population.size(); ++i) {
    if (_population[i].getScore() > best) {
      best = _population[i].getScore();
      bestId = i;
    }
  }
  return _population[bestId];
}

/*
** Retrieve the individual with the worst fitness score in the population
** should be changed in order to sort the population and return n last elements
**
** @return worst individual
*/
Individual      &Population::getWorst() {
  fitness_t	worst = _population[0].getScore();
  uint16_t	worstId = 0;

  for (uint16_t i = 0; i < _population.size(); ++i) {
    if (_population[i].getScore() < worst) {
      worst = _population[i].getScore();
      worstId = i;
    }
  }
  return _population[worstId];
}

/*
** Walk through the population to display each individual on console
*/
void            Population::termDisplay() const {
  for (auto &individual : _population) {
    individual.termDisplay();
  }
}

/*
** Append individual into the population
**
** @param individual: Individual object
*/
void            Population::addIndividual(const Individual &individual) {
  _population.push_back(individual);
}

/*
** Retrieve the population size
**
** @return population size
*/
uint		Population::size() const {
  return _population.size();
}

/*
** Population vector accessor
**
** @param index: uint that specifies a position in an array
** @return Individual object at position 'index'
*/
const Individual	&Population::operator[](uint index) const {
  return _population[index];
}
