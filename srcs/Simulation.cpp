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
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC << std::endl;

  simxInt ret;

  for (uint16_t i = 0; i < _maxTries; i++) {
    std::cout << "Population size: " << _population.getPopulation().size() << std::endl;
    // evaluate
    std::cout << "Global fitness: " << _population.evaluateBatch() << std::endl;
    // selection
    _population.termDisplay();
    this->breedingSeason();
    std::cout << "population after selection" << std::endl;
     _population.termDisplay();
    // elites
    // crossover
    // mutation

    // for each individual start a simulation and execute the dna on vrep
    for (uint16_t j = 0; j < _population.getPopulation().size(); j++) {
      ret = simxStartSimulation(_clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStartSimulation error: " << ret << std::endl;
        return ret;
      }

      _population.getPopulation()[j]->termDisplay();
      // do action in vrep
      ret = _robot.doActions(_population.getPopulation()[j]->getDna());
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

void
Simulation::breedingSeason() {
  // suming the weights for a weighted random
  fitness_t weightsSum = 0;
  for (auto individual : _population.getPopulation()) {
    weightsSum += individual->getScore();
  }

  // container for new generation
  auto newPopulationGeneration = Population(0);

  // repeat mating as many time as needed to ensure constant population size
  for (uint i = 0; i < this->_population.getPopulation().size(); ++i) {
    auto couple = this->makeCouple(weightsSum);
    auto child = couple.first->mate(couple.second);
//    newPopulationGeneration.getPopulation().push_back(couple.first);
    newPopulationGeneration.addChild(couple.first);
  }
  _population = newPopulationGeneration;
}

couple_t
Simulation::makeCouple(fitness_t weightsSum) {
  Individual *leftMate, *rightMate = nullptr;

  // repeating until we got two different individuals that can mate
  while (leftMate == rightMate || !leftMate || !rightMate) {
    // get randomly a value corresponding to the weighted cursor
    fitness_t randomWeightIndex = RandomGenerator::getInstance().d_between(0, weightsSum);

    // searching for the individual aimed by the cursor
    // (when the random weighted cursor goes under or is equal to 0)
    for (Individual *individual : _population.getPopulation()) {
      randomWeightIndex -= individual->getScore();
      if (randomWeightIndex <= 0) {
        if (leftMate)   rightMate = individual;
        else            leftMate = individual;
        break;
      }
    }
  }

  // returning the couple
  return couple_t(leftMate, rightMate);
}
