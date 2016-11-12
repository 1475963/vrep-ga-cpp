#pragma once

#include <cstdlib>
#include <iostream>
#include "types.hh"
#include "RandomGenerator.hpp"
#include "Robot.hh"

class Individual {

public:
  Individual() = default;
  Individual(uint16_t length);
  Individual(const dna_t &dna);

  Individual	&operator=(const Individual &other);
  void		initialize(uint16_t length);
  fitness_t	evaluate();
  fitness_t evaluate(simxFloat *prevPos, simxFloat *nextPos);
  void		mutate();
  void		termDisplay()			const;
  fitness_t	getScore()			const;
  const dna_t	&getDna()			const;
  void		setScore(fitness_t fitness);
  uint		dnaSize()			const;
  bool		initialized()			const;
  action_t	getGene(uint index)		const;
  void		addGene(const action_t &action);

private:
  static const uint8_t  _mutationRate = 5;
  dna_t                 _dna;
  fitness_t             _score = 0;
};

bool operator==(const Individual& lhs, const Individual& rhs);
