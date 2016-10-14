#include "Simulation.hh"

Simulation::Simulation()
: _population(Population(maxPop))
//:  _connection(VrepConnection("127.0.0.1", 19997, 1, 1, 5000, 5))
{}

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
  simxInt clientID = simxStart("127.0.0.1", 19997, true, true, 5000, 5);
  if (clientID == -1) {
    std::cerr << "simxStart error" << std::endl;
    return -1;
  }
  
  const Robot robot(clientID);

  for (uint16_t i = 0; i < maxTries; i++) {
    std::cout << "Population size: " << _population.getPopulation().size() << std::endl;
    // evaluate
    std::cout << "Global fitness: " << _population.evaluateBatch() << std::endl;
    // selection
    // elites
    // crossover
    // mutation

    // for each individual start a simulation and execute the dna on vrep
    for (uint16_t j = 0; j < _population.getPopulation().size(); j++) {
      ret = simxStartSimulation(clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStartSimulation error: " << ret << std::endl;
        return ret;
      }

      // do action in vrep
      ret = robot.doActions(_population.getPopulation()[i].getDna());
      if (ret != 0) {
        std::cerr << "Robot::doActions error: " << ret << std::endl;
        return ret;
      }

      ret = simxStopSimulation(clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStopSimulation error: " << ret << std::endl;
        return ret;
      }
    }

    // logs ?
    _population.termDisplay();
  }
  std::cout << "## END" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC;
  return (0);
}
