#pragma once

#include <vector>
#include <string>
#include <iostream> // cout / cerr
#include <unistd.h> // sleep

#define _USE_MATH_DEFINES // pi constant
#include <math.h> // pi constant

extern "C" {
#include "extApi.h"
}
#include "types.hh"

struct move_t {
public:
  move_t(const simxChar *bodyPart, const std::string &description, double intensity);

  const simxChar *_bodyPart;
  const std::string _description;
  const double _intensity;
  simxInt _handler;
};

typedef std::vector<move_t > move_list_t;

class Robot {
public:
  Robot(simxInt clientID);

  static inline action_t highestAction() {
    return actions.size() - 1;
  }

  simxInt doActions(const dna_t &dna) const;

private:
  simxInt _clientID;
  static const move_list_t actions;
  static uint8_t _nextId;
  std::vector<simxInt> _handlers;
  uint8_t _id;
};
