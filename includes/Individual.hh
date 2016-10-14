#pragma once

#include <cstdlib>
#include <iostream>
#include "types.hh"
#include "RandomGenerator.hpp"

class Individual {

public:
  Individual();
  Individual(uint16_t);
  Individual(dna_t);
  Individual  &initialize(uint16_t);
  fitness_t   evaluate();
  void        termDisplay();
  fitness_t   getScore() const;
  dna_t       &getDna();
  void        setScore(fitness_t);
  void        setDna(dna_t);

private:
  const uint16_t  _defaultMaxActions = 10;
  dna_t           _dna;
  fitness_t       _score = 0;

};
