#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <unistd.h>
#include <random>

#define M_PI (3.141592)

using namespace std;

extern "C" {
  #include "extApi.h"
}

double  toRadians(double degree) {
  return (2 * M_PI * (degree / 360));
}

int main(int argc, char **argv) {
  cout << "start !" << endl;

  int clientID = simxStart((simxChar*)"127.0.0.1", 19997, true, true, 5000, 5);

  if (clientID != -1) {
    cout << "Connected to remote API server !" << endl;

    simxInt opmode = simx_opmode_oneshot_wait;

    simxInt wristHandle,
            elbowHandle,
            shoulderHandle,
            robotHandle,
            leftWheelHandle,
            rightWheelHandle;
    uint8_t ret1 = simxGetObjectHandle(clientID, "WristMotor", &wristHandle, opmode),
            ret2 = simxGetObjectHandle(clientID, "ElbowMotor", &elbowHandle, opmode),
            ret3 = simxGetObjectHandle(clientID, "ShoulderMotor", &shoulderHandle, opmode),
            ret4 = simxGetObjectHandle(clientID, "2W1A", &robotHandle, opmode),
            ret5 = simxGetObjectHandle(clientID, "LeftWheelJoint", &leftWheelHandle, opmode),
            ret6 = simxGetObjectHandle(clientID, "RightWheelJoint", &rightWheelHandle, opmode);

    if (!ret1 && !ret2 && !ret3 && !ret4 && !ret5 && !ret6) {
      float pos[3],
            orient[3];

      uint16_t awrist,
               aelbow,
               ashoulder;

      uint8_t posRet,
              orientRet;
      
      float pwrist,
            pelbow,
            pshoulder;

      random_device rd;
      mt19937 gen(rd());
      uniform_int_distribution<> dis(0, 359);
      
      cout << "Objects handled correctly" << endl;
      srand(time(0));

      for (uint8_t i = 0; i < 3; i++) {
        simxStartSimulation(clientID, opmode);
        cout << "----- Simulation started -----" << endl;

        posRet = simxGetObjectPosition(clientID, robotHandle, -1, pos, simx_opmode_streaming);
        cout << "2w1a position: (x = " << pos[0] << ", y = " << pos[1] << ")" << endl;
        orientRet = simxGetObjectOrientation(clientID, robotHandle, -1, orient, simx_opmode_streaming);
        cout << "2w1a orientation: (x = " << orient[0] << ", y = " << orient[1] << ", z = " << orient[2] << ")" << endl;

        for (uint8_t j = 0; j < 20; j++) {
          awrist = dis(gen);
          aelbow = dis(gen);
          ashoulder = dis(gen);

          cout << "Motors target positions: " << ashoulder << " " << aelbow << " " << awrist << endl;

          simxSetJointTargetPosition(clientID, wristHandle, toRadians(awrist), opmode);
          simxSetJointTargetPosition(clientID, elbowHandle, toRadians(aelbow), opmode);
          simxSetJointTargetPosition(clientID, shoulderHandle, toRadians(ashoulder), opmode);

          sleep(3);

          simxGetJointPosition(clientID, wristHandle, &pwrist, opmode);
          simxGetJointPosition(clientID, elbowHandle, &pelbow, opmode);
          simxGetJointPosition(clientID, shoulderHandle, &pshoulder, opmode);

          cout << "Motos reached positions: " << ashoulder << " " << aelbow << " " << awrist;

          posRet = simxGetObjectPosition(clientID, robotHandle, -1, pos, simx_opmode_buffer);
          cout << "2w1a position: (x = " << pos[0] << ", y = " << pos[1] << ")" << endl;
          orientRet = simxGetObjectOrientation(clientID, robotHandle, -1, orient, simx_opmode_buffer);
          cout << "2w1a orientation: (x = " << orient[0] << ", y = " << orient[1] << ", z = " << orient[2] << ")" << endl;
        }

        simxStopSimulation(clientID, opmode);
        sleep(1);
        cout << "----- Simulation ended -----" << endl;
      }

      simxFinish(clientID);
    }
  }
  return (0);
}
