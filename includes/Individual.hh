#pragma once

#include "types.hh"

struct Individual {
  dna_t dna;
  fitness_t score;

  Individual();
  Individual(dna_t);
  ~Individual();
};
