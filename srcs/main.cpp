#include <iostream>

#include "Simulation.hh"
#include "Robot.hh"

extern "C" {
#include "extApi.h"
}
using namespace std;

int main() {
  auto simulation = Simulation();
  simulation.run();
  /*
  simxInt ret;
  simxInt clientID = simxStart("127.0.0.1", 19997, true, true, 5000, 5);

  if (clientID == -1) {
    cerr << "simxStart error" << endl;
    return -1;
  }

  ret = simxStartSimulation(clientID, simx_opmode_oneshot_wait);
  if (ret != 0) {
    cerr << "simxStartSimulation error: " << ret << endl;
    return ret;
  }

  const dna_t dna = {
    0,9,4,6,7,8,1,0,30,3,4,8,9,4,7,9,1,3,3,0,3,3,5,8,7,50
  };
  const Robot robot(clientID);

  ret = robot.doActions(dna);
  if (ret != 0) {
    cerr << "Robot::doActions error: " << ret << endl;
    return ret;
  }

  ret = simxStopSimulation(clientID, simx_opmode_oneshot_wait);
  if (ret != 0) {
    cerr << "simxStopSimulation error: " << ret << endl;
    return ret;
  }
  */
  return 0;
}
