#pragma once

#include <cstdint>
#include <vector>

extern "C" {
#include "extApi.h"
}

class Individual;

typedef uint8_t robot_id_t;
typedef uint8_t action_t;
typedef double  fitness_t;

typedef std::vector<action_t>             dna_t;
typedef std::pair<Individual, Individual> couple_t;
