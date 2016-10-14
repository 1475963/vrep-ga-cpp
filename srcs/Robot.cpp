#include "Robot.hh"

using namespace std;

// list of possible moves
const move_list_t Robot::actions = {
  {"LeftWheelJoint",	"Move left wheel forward",	1},
  {"LeftWheelJoint",	"Move left wheel backward",	-1},
  {"RightWheelJoint",	"Move right wheel forward",	1},
  {"RightWheelJoint",	"Move right wheel backward",	-1},
  {"ShoulderMotor",	"Move shoulder forward",	1},
  {"ShoulderMotor",	"Move shoulder backward",	-1},
  {"ElbowMotor",	"Move elbow forward",		1},
  {"ElbowMotor",	"Move elbow backward",		-1},
  {"WristMotor",	"Move wrist forward",		1},
  {"WristMotor",	"Move wrist backward",		-1}
};

// id of the next robot created
uint8_t Robot::_nextId = 0;

/***** move_t *****/

/*
** movet_t constructor, describe a movement
**
** @param bodyPart: Joint on which to apply the move
** @param description: move description
** @param intensity: move intensity, that is to say it's value
*/
move_t::move_t(const simxChar *bodyPart, const std::string &description, double intensity) :
  _bodyPart(bodyPart), _description(description), _intensity(intensity) {
}

/***** Robot *****/

/*
** Robot constructor - initialise its parameters and handler
** handler initialisation will then be moved into a load() or reload() function
** clientId can also be a static variable in the simulation instead of a paramet
**
** @param clientID: client on which to apply moves
*/
Robot::Robot(simxInt clientID) :
  _clientID(clientID), _handlers(highestAction()), _id(_nextId) {
  ++_nextId;
  // here we throw but it's not a problem as this will later be done in another function
  for (int i = 0; i < highestAction(); i++)
    if (simxGetObjectHandle(_clientID, Robot::actions.at(i)._bodyPart, &_handlers[i], simx_opmode_blocking) != 0)
      throw "err";
  cout << "Robot " << (int)_id << " created" << endl;
}

/*
** Execute a serie of action encoded in a DNA sequence
**
** @param dna: DNA sequence in which moves are encoded
** @return: an error or success code as given by V-REP
*/
simxInt		Robot::doActions(const dna_t &dna) const {
  simxInt	ret;

  for (const auto &action : dna) {
    if (action > highestAction()) {
      cerr << "Robot " << (int)_id << " can't execute " << (int)action << ": It does not exists!" << endl;
      continue;
    }
    cout << "Robot " << (int)_id << " executing " << actions.at(action)._description << endl;
    ret = simxSetJointTargetPosition(_clientID, _handlers[action], Robot::actions.at(action)._intensity, simx_opmode_oneshot);
    if (ret != 0 && ret != 1)
      return ret;
    sleep(2);
  }
  return 0;
}
