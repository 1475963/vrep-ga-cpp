#include "Individual.hh"

Individual::Individual() {
  this->initialize(_defaultMaxActions);
}

Individual::Individual(uint16_t maxActions) {
  this->initialize(maxActions);
}

Individual::Individual(dna_t dna) {
  this->setDna(dna);
}

Individual  &Individual::initialize(uint16_t maxActions) {
  RandomGenerator &rg = RandomGenerator::getInstance();

  for (uint16_t i = 0; i < maxActions; i++) {
    _dna.push_back(rg.i_between(0, 9));
  }

  return (*this);
}

fitness_t   Individual::evaluate() {
  RandomGenerator &rg = RandomGenerator::getInstance();

  this->setScore(rg.d_between(0, 1));
  return (this->getScore());
}

void        Individual::termDisplay() {
  for (uint16_t i = 0; i < this->getDna().size(); i++) {
    std::cout << static_cast<int>(this->getDna()[i]);
  }
}

fitness_t   Individual::getScore() const {
  return (_score);
}

dna_t       &Individual::getDna() {
  return (_dna);
}

void        Individual::setScore(fitness_t fitness) {
  _score = fitness;
}

void        Individual::setDna(dna_t dna) {
  _dna = dna;
}
