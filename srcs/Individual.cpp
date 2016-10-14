#include "Individual.hh"

Individual::Individual() {
  this->initialize(Robot::highestAction());
}

Individual::Individual(uint16_t maxActions) {
  this->initialize(maxActions);
}

Individual::Individual(const dna_t &dna) {
  _dna = dna;
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

  _score = rg.d_between(0, 1);
  return (_score);
}

void        Individual::termDisplay() {
  std::cout << "Fitness: " << _score << ", dna: ";
  for (uint16_t i = 0; i < _dna.size(); i++) {
    std::cout << static_cast<int>(_dna[i]);
  }
  std::cout << std::endl;
}

fitness_t   Individual::getScore() const {
  return (_score);
}

dna_t       Individual::getDna() const {
  return (_dna);
}

void        Individual::setScore(fitness_t fitness) {
  _score = fitness;
}

void        Individual::setDna(dna_t dna) {
  _dna = dna;
}

void Individual::mutate() {
  RandomGenerator &rg = RandomGenerator::getInstance();

  for (unsigned int i = 0; i < _dna.size(); i++)
    if (rg.i_between(0, 100) < _mutationRate)
      _dna[i] = rg.i_between(0, Robot::highestAction());
}
