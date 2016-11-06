#include "Individual.hh"

/*
** Check whether 2 individuals are equals by comparing their DNA
**
** @param lhs: left individual
** @param rhs: right individual
** @return whether individuals are equals
*/
bool operator==(const Individual& lhs, const Individual& rhs) {
  return lhs.getDna() == rhs.getDna();
}

/*
** Individual constructor generating a DNA of size 'length'
**
** @param length: DNA size
*/
Individual::Individual(uint16_t length) {
  initialize(length);
}

/*
** Individual constructor creating a copy of a DNA
**
** @param dna: DNA to copy
*/
Individual::Individual(const dna_t &dna) {
  _dna = dna;
}

/*
** Check whether an individual is initialised,
** that is to say if an individual has a DNA
**
** @return: whether the inidividual is initialised
*/
bool Individual::initialized() const {
  return _dna.size() != 0;
}

/*
** Copy constructor
**
** @param other: Individual to copy
** @return: Copied individual
*/
Individual  &Individual::operator=(const Individual &other) {
  if (this != &other) {
    _dna = other.getDna();
    _score = other.getScore();
  }
  return *this;
}

/*
** Initialise individual DNA randomly
**
** @param length: DNA length
*/
void  Individual::initialize(uint16_t length) {
  RandomGenerator &rg = RandomGenerator::getInstance();

  _dna.clear();

  for (uint16_t i = 0; i < length; i++) {
    _dna.push_back(rg.i_between(0, Robot::highestAction()));
  }
  std::cout << "DNA content: " << std::endl;
  termDisplay();
}

/*
** fitness function
**
** @return: Individual fitness
*/
fitness_t   Individual::evaluate() {
  RandomGenerator &rg = RandomGenerator::getInstance();

  _score = rg.d_between(0, 1);
  return (_score);
}

/*
** Print an individual DNA
*/
void        Individual::termDisplay() const {
  for (uint8_t action : _dna) {
    std::cout << static_cast<int>(action) << ".";
  }
  std::cout << std::endl;
}

/*
** get individual fitness
**
** @return: individual fitness
*/
fitness_t   Individual::getScore() const {
  return _score;
}

/*
** get individual DNA
**
** @return: a const reference to the individual fitness
*/
const dna_t       &Individual::getDna() const {
  return _dna;
}

/*
** helper to get an individual DNA size
**
** @return: DNA size
*/
uint	Individual::dnaSize() const {
  return _dna.size();
}

/*
** Set an individual fitness
**
** @param fitness: new individual fitness
*/
void        Individual::setScore(fitness_t fitness) {
  _score = fitness;
}

/*
** Randomly mutate an individual's DNA
*/
void		Individual::mutate() {
  RandomGenerator &rg = RandomGenerator::getInstance();

  for (unsigned int i = 0; i < _dna.size(); i++)
    if (rg.i_between(0, 100) < _mutationRate)
      _dna[i] = rg.i_between(0, Robot::highestAction());
}

/*
** Get part of an individual's DNA
**
** @param index: gene to retreive
** @return: gene at index 'index'
*/
action_t	Individual::getGene(uint index) const {
  return _dna.at(index);
}

/*
** Add an action to an individual's DNA
**
** @param action: new individual's action
*/
void		Individual::addGene(const action_t &action) {
  _dna.push_back(action);
}
