#include "Population.hh"

Population::Population(uint16_t maxPop) {
  initialize(maxPop);
}

void  Population::initialize(uint16_t maxPop) {
  RandomGenerator &rg = RandomGenerator::getInstance();

  _population.clear();
  _population.resize(maxPop);

  for (uint16_t i = 0; i < maxPop; i++) {
    _population[i].initialize(rg.i_between(1, 10));
  }


  std::cout << "resume population content: " << std::endl;
  termDisplay();
}

fitness_t       Population::evaluateBatch() {
  fitness_t     globalFitness = 0;

  for (auto &individual : _population) {
    individual.evaluate();
    globalFitness += individual.getScore();
  }

  return (globalFitness / (_population.size() > 0 ? _population.size() : 1));
}

void            Population::mutateBatch() {
  for (auto &individual : _population) {
    individual.mutate();
  }
}

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

void            Population::termDisplay() const {
  for (auto &individual : _population) {
    individual.termDisplay();
  }
}

void            Population::addChild(const Individual &individual) {
  _population.push_back(individual);
}

uint		Population::size() const {
  return _population.size();
}

const Individual	&Population::operator[](uint index) const {
  return _population[index];
}
