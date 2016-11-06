#pragma once

#include <cstdint>
#include <vector>

class Individual;

typedef uint8_t action_t;
typedef std::vector<action_t> dna_t;
typedef double fitness_t;
typedef std::pair<Individual, Individual> couple_t;
