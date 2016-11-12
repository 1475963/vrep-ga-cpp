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

  #pragma omp parallel for
  for (uint16_t i = 0; i < maxPop; i++) {
    _population.at(i).initialize(rg.i_between(20, 50));
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

/**
 * Sort the population from the best to the worst individual
 */
void		Population::sort() {
  std::sort(_population.begin(), _population.end(), [](const Individual i1, const Individual i2) {
      return i1.getScore() > i2.getScore();
    });
}

/*
** Retrieve the individual with the best fitness score in the population
**
** @return best individual
*/
Individual      &Population::getElite() {
  return _population.at(0);
}

/*
** Retrieve the individual with the worst fitness score in the population
**
** @return worst individual
*/
Individual      &Population::getWorst() {
  return _population.at(_population.size() - 1);
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
  #pragma omp critical
  {
    _population.push_back(individual);
  }
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
Individual	&Population::operator[](uint index) {
  return _population[index];
}

/*
** Population vector accessor
**
** @param index: uint that specifies a position in an array
** @return Individual object at position 'index'
*/
const Individual	&Population::at(uint index) const {
  return _population[index];
}
