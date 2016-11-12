#include "Robot.hh"

using namespace std;

/*
** Movement constructor, describe a movement
**
** @param bodyPart: Joint on which to apply the move
** @param description: move description
** @param intensity: move intensity, that is to say it's value
*/
Robot::Movement::Movement(const simxChar *bodyPart, const std::string &description, double intensity) :
  bodyPart(bodyPart), description(description), intensity(intensity) {
}

/***** Robot *****/

const Robot::movement_list_t Robot::_movements = {
  { "LeftWheelJoint",  "Move left wheel forward",    1 },
  { "LeftWheelJoint",  "Move left wheel backward",  -1 },
  { "RightWheelJoint", "Move right wheel forward",   1 },
  { "RightWheelJoint", "Move right wheel backward", -1 },
  { "ShoulderMotor",   "Move shoulder forward",      1 },
  { "ShoulderMotor",   "Move shoulder backward",    -1 },
  { "ElbowMotor",      "Move elbow forward",         1 },
  { "ElbowMotor",      "Move elbow backward",       -1 },
  { "WristMotor",      "Move wrist forward",         1 },
  { "WristMotor",      "Move wrist backward",       -1 }
};

/*
** Robot constructor - initialise its parameters and handler
** handler initialisation will then be moved into a load() or reload() function
** clientId can also be a static variable in the simulation instead of a paramet
**
** @param clientID: client on which to apply moves
*/
Robot::Robot(simxInt clientID, robot_id_t id) :
  _clientID(clientID),
  _id(id) {

  const char *robotName = ("2WA1#" + std::to_string(_id)).c_str();
  auto err = simxGetObjectHandle(
    _clientID,
    robotName,
    &_robotHandler,
    simx_opmode_blocking
  );

  if (err == -1) {
    auto errMsg = string("Can't get object handle for ") + robotName;
    throw errMsg.c_str();
  }

  // Retrieving movements handlers
  for (const auto &movement : _movements) {
    // First creating articulation identifier
    const simxChar *completeBodyPart = (movement.bodyPart + '#' + std::to_string(_id)).c_str();

    // Retrieving handler by distant assignation
    movement_handler_t movementHandler;
    auto handlerAssignationResult = simxGetObjectHandle(
      _clientID,           // connection fd
      completeBodyPart,    // articulation identifier
      &movementHandler,    // pointer on the variable receiving the handler
      simx_opmode_blocking // flag
    );
    if (handlerAssignationResult == -1) {
      auto errorMessage = string("Can't get object handle for ") + completeBodyPart;
      throw errorMessage.c_str();
    }

    // Adding the handler to the list of known ones, in the same order as the movements
    _movementHandlers.push_back(movementHandler);
  }
}

/*
** Execute a serie of action encoded in a DNA sequence
**
** @param dna: DNA sequence in which moves are encoded
** @return: an error or success code as given by V-REP
*/
simxInt		Robot::doActions(const dna_t &dna) const {
  for (const auto &action : dna) {
    if (action > highestAction()) {
      cerr << "Robot " << (int)_id << " can't execute " << (int)action << ": It does not exists!" << endl;
      continue;
    }

    auto movementResult = simxSetJointTargetPosition(
      _clientID,
      _movementHandlers[action],
      (Robot::_movements[action].intensity * M_PI) / 180,
      simx_opmode_blocking
    );
    if (movementResult == -1) {
      cerr
        << "Movement failed for action " << action
        << " (movement id" << _movementHandlers[action] << ")"
        << "on robot " << std::to_string(_id)
        << endl;
      return movementResult;
    }
  }
  return 0;
}

/*
** Retrieve the current position of the robot
**
** @param position: an array of 3 floats in which space positions (x, y, z) are stored
** @return: an error or success code as given by V-REP
*/
simxInt   Robot::getPosition(simxFloat *position, const simxInt objectRef) const {
  return simxGetObjectPosition(
    _clientID,
    _robotHandler,
    objectRef,
    position,
    simx_opmode_blocking
  );
}
