#include <VrepConnection.hh>

VrepConnection::VrepConnection(
    simxChar *address,
    simxInt connection_port,
    simxUChar wait_until_connected,
    simxUChar do_not_reconnect_once_disconnected,
    simxInt time_out_in_ms,
    simxInt comm_thread_cycle_in_ms
) : _address(address),
    _connection_port(connection_port),
    _wait_until_connected(wait_until_connected),
    _do_not_reconnect_once_disconnected(do_not_reconnect_once_disconnected),
    _time_out_in_ms(time_out_in_ms),
    _communication_thread_cycle_in_ms(comm_thread_cycle_in_ms)
{
    // etablishing connection
    _connection = simxStart(
        _address, _connection_port,
        _wait_until_connected, _do_not_reconnect_once_disconnected, _time_out_in_ms,
        _communication_thread_cycle_in_ms
    );

    if (_connection < 0) throw;
}

VrepConnection::~VrepConnection() {
    simxFinish(_connection);
}
