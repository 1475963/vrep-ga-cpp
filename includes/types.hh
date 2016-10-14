#pragma once

#include <cstdint>
#include <vector>

struct Individual;

typedef uint8_t action_t;
typedef std::vector<action_t> dna_t;
typedef float fitness_t;
typedef std::vector<Individual *> Population;
typedef std::pair<Individual *, Individual *> couple_t;
