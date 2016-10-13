#pragma once

#include "extApi.h"
#include "Robot.hh"

class VrepConnection
{
public:
  VrepConnection(simxChar *, simxInt, simxUChar, simxUChar, simxInt, simxInt);
  ~VrepConnection();

  Robot *		instantiateRobot();
private:
  simxChar * _address;
  simxInt    _connection_port;
  simxUChar  _wait_until_connected;
  simxUChar  _do_not_reconnect_once_disconnected;
  simxInt    _time_out_in_ms;
  simxInt    _communication_thread_cycle_in_ms;

  simxInt    _connection;
};
