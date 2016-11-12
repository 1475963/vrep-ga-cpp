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
** fitness function, takes two position states and computes a ratio
**
** @return: Individual fitness
*/
fitness_t   Individual::evaluate(simxFloat *prevPos, simxFloat *nextPos) {
  float x1 = prevPos[0],
        y1 = prevPos[1],
        x2 = nextPos[0],
        y2 = nextPos[1];

  if (isnan(x1) == true) {
    x1 = 0;
  }
  if (isnan(y1) == true) {
    y1 = 0;
  }
  if (isnan(x2) == true) {
    x2 = 0;
  }
  if (isnan(y2) == true) {
    y2 = 0;
  }

  std::cout << "X1: " << x1 << "\tY1: " << y1 << std::endl;
  std::cout << "X2: " << x2 << "\tY2: " << y2 << std::endl;

  float v1 = std::sqrt(pow(x1, 2) + pow(y1, 2)),
        v2 = std::sqrt(pow(x2, 2) + pow(y2, 2));

  std::cout << "First vector: " << v1 << std::endl;
  std::cout << "Second vector : " << v2 << std::endl;

  float moveVector = fabs(v1 - v2);

  std::cout << "Movement vector" << moveVector << std::endl;

  moveVector = 1 / moveVector;

  std::cout << "Normalized: " << moveVector << std::endl;

  // get the opposite because selection has an ascending order behavior (the bigger, the better)
  moveVector *= -1;

  std::cout << "Adjust for implementation needs: " << moveVector << std::endl;

  _score = moveVector;

  return moveVector;
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
