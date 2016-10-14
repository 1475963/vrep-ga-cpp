#include "Individual.hh"

Individual::Individual() {
  _dna = dna_t();
  _score = 0;
}

Individual::Individual(const dna_t &dna) {
  _dna = dna;
}

void Individual::mutate() {
  RandomGenerator &rg = RandomGenerator::getInstance();

  for (unsigned int i = 0; i < _dna.size(); i++)
    if (rg.i_between(0, 100) < _mutationRate)
      _dna[i] = rg.i_between(0, Robot::highestAction());
}
