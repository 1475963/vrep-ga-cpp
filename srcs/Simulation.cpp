#include "Simulation.hh"

Simulation::Simulation()
: _connection(VrepConnection("127.0.0.1", 19997, 1, 1, 5000, 5))
{}

Simulation::~Simulation()
{}

void
Simulation::run() {
}

