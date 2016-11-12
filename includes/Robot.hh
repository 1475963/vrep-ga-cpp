#pragma once

#include <vector>
#include <string>
#include <iostream> // cout / cerr
#include <unistd.h> // sleep

#define _USE_MATH_DEFINES // pi constant
#include <cmath> // pi constant

#include "types.hh"

class Robot {
  struct Movement;
  typedef std::vector<Robot::Movement>      movement_list_t;

  typedef simxInt                           movement_handler_t;
  typedef std::vector<movement_handler_t>   movement_handler_list_t;

  static const movement_list_t _movements;

public:
  static inline action_t   highestAction() { return _movements.size() - 1; }

private:
  simxInt                 _clientID;
  robot_id_t              _id;
  movement_handler_list_t _movementHandlers;
  simxInt                 _robotHandler;

public:
  Robot(simxInt clientID, robot_id_t robotId);

  inline robot_id_t getId() { return _id; }

  simxInt doActions(const dna_t &dna) const;

  simxInt getPosition(simxFloat *position, simxInt objectRef) const;

private:
  struct Movement {
    Movement(const simxChar *bodyPart, const std::string &description, double intensity);

    const std::string bodyPart;
    const std::string description;
    const double      intensity;
  };
};
