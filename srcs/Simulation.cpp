#include "Simulation.hh"

Simulation::Simulation()
: _population(Population(_maxPop)),
  _clientID(simxStart("127.0.0.1", 19997, true, true, 5000, 5)),
  _robot(Robot(_clientID))
//:  _connection(VrepConnection("127.0.0.1", 19997, 1, 1, 5000, 5))
{
  if (_clientID == -1) {
    throw "simxStart error";
  }
}

Simulation::~Simulation()
{}

/**
 *  Use a clock object ?
 *  Individual length and value limits are hardcoded :(
 */
int Simulation::run() {
  clock_t start = clock();

  std::cout << "## START" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC;

  simxInt ret;

  for (uint16_t i = 0; i < _maxTries; i++) {
    std::cout << "Population size: " << _population.getPopulation().size() << std::endl;
    // evaluate
    std::cout << "Global fitness: " << _population.evaluateBatch() << std::endl;
    // selection
    // elites
    // crossover
    // mutation
    _population.termDisplay();

    // for each individual start a simulation and execute the dna on vrep
    for (uint16_t j = 0; j < _population.getPopulation().size(); j++) {
      ret = simxStartSimulation(_clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStartSimulation error: " << ret << std::endl;
        return ret;
      }

      _population.getPopulation()[j].termDisplay();
      // do action in vrep
      ret = _robot.doActions(_population.getPopulation()[j].getDna());
      if (ret != 0) {
        std::cerr << "Robot::doActions error: " << ret << std::endl;
        return ret;
      }

      ret = simxStopSimulation(_clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStopSimulation error: " << ret << std::endl;
        return ret;
      }
    }

    // logs ?
  }
  std::cout << "## END" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC;
  return (0);
}
