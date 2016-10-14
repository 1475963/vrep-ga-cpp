#pragma once

#include <cstdlib>
#include <cstdint>

#include "types.hh"
#include "RandomGenerator.hpp"
#include "Robot.hh"

struct Individual {
public:
  dna_t _dna;
  fitness_t _score;

  Individual();
  Individual(const dna_t &_dna);

  void mutate();
private:
  static const uint8_t _mutationRate = 5;
};
