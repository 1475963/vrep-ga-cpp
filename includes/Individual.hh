#pragma once

#include <cstdlib>
#include <iostream>
#include "types.hh"
#include "RandomGenerator.hpp"
#include "Robot.hh"

class Individual {

public:
  Individual();
  Individual(uint16_t length);
  Individual(const dna_t &dna);
  void        initialize(uint16_t length);
  fitness_t   evaluate();
  void        mutate();
  Individual *mate(Individual *);
  void        termDisplay() const;
  fitness_t   getScore() const;
  dna_t       getDna() const;
  void        setScore(fitness_t fitness);
  void        setDna(const dna_t &dna);

private:
  static const uint8_t  _mutationRate = 5;
  static const uint16_t _defaultMaxLength = 10;
  dna_t                 _dna;
  fitness_t             _score = 0;
};
