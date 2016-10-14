#pragma once

#include <vector>
#include <string>
#include <iostream> // cout / cerr
#include <unistd.h> // sleep

#include "extApi.h"

#include "types.hh"

struct move_t {
public:
  move_t(const std::string &bodyPart, const std::string &description, double intensity);

  const std::string _bodyPart;
  const std::string _description;
  const double _intensity;
};

typedef std::vector<move_t > move_list_t;

class Robot {
public:
  Robot(simxInt clientID);
  static inline action_t highestAction();
  simxInt doActions(const dna_t &dna) const;

private:
  simxInt _clientID;
  static const move_list_t actions;
  static uint8_t _nextId;
  uint8_t _id;
};
