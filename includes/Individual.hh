#pragma once

#include <cstdlib>
#include "types.hh"

struct Individual {
  dna_t dna;
  fitness_t score;

  Individual();
  Individual(dna_t);
  ~Individual();

  Individual *mate(Individual *);
};
