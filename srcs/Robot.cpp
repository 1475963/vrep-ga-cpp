#include "Robot.hh"

using namespace std;

const move_list_t Robot::actions = {
  {"LeftWheelJoint", "Move left wheel forward", 1},
  {"LeftWheelJoint", "Move left wheel backward", -1},
  {"RightWheelJoint", "Move right wheel forward", 1},
  {"RightWheelJoint", "Move right wheel backward", -1},
  {"ShoulderMotor", "Move shoulder forward", 1},
  {"ShoulderMotor", "Move shoulder backward", -1},
  {"ElbowMotor", "Move elbow forward", 1},
  {"ElbowMotor", "Move elbow backward", -1},
  {"WristMotor", "Move wrist forward", 1},
  {"WristMotor", "Move wrist backward", -1}
};

uint8_t Robot::_nextId = 0;

/***** move_t *****/

move_t::move_t(const simxChar *bodyPart, const std::string &description, double intensity) :
  _bodyPart(bodyPart), _description(description), _intensity(intensity)
{}

/***** Robot *****/

Robot::Robot(simxInt clientID) :
  _clientID(clientID), _handlers(highestAction()), _id(_nextId) {
  ++_nextId;
  // here we throw but it's not a problem as this will later be done in another function
  for (int i = 0; i < highestAction(); i++)
    if (simxGetObjectHandle(_clientID, Robot::actions.at(i)._bodyPart, &_handlers[i], simx_opmode_blocking) != 0)
      throw "err";
  cout << "Robot " << (int)_id << " created" << endl;
}

action_t	Robot::highestAction() {
  return actions.size();
}

simxInt	Robot::doActions(const dna_t &dna) const {
  simxInt ret = 1;

  (void)ret;
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
