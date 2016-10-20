#include "Individual.hh"

Individual::Individual() {
  this->initialize(_defaultMaxLength);
}

Individual::Individual(uint16_t length) {
  this->initialize(length);
}

Individual::Individual(const dna_t &dna) {
  _dna = dna;
}

void  Individual::initialize(uint16_t length) {
  RandomGenerator &rg = RandomGenerator::getInstance();

  _dna.clear();

  for (uint16_t i = 0; i < length; i++) {
    _dna.push_back(rg.i_between(0, Robot::highestAction()));
  }
}

fitness_t   Individual::evaluate() {
  RandomGenerator &rg = RandomGenerator::getInstance();

  _score = rg.d_between(0, 1);
  return (_score);
}

Individual *
Individual::mate(Individual *partner) {
  (void)partner;
  return nullptr;
}

void        Individual::termDisplay() const {
  for (uint8_t action : _dna) {
    std::cout << static_cast<int>(action);
  }
  std::cout << std::endl;
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

void        Individual::setDna(const dna_t &dna) {
  _dna = dna;
}

void Individual::mutate() {
  RandomGenerator &rg = RandomGenerator::getInstance();

  for (unsigned int i = 0; i < _dna.size(); i++)
    if (rg.i_between(0, 100) < _mutationRate)
      _dna[i] = rg.i_between(0, Robot::highestAction());
}
