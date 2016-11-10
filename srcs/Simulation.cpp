#include "Simulation.hh"

static int
roundUp(int numToRound, int multiple) {
  if (multiple == 0)
    return numToRound;

  int remainder = numToRound % multiple;
  if (remainder == 0)
    return numToRound;

  return numToRound + multiple - remainder;
}

Simulation::Simulation() :
  _maxRobots(omp_get_max_threads()),
  _maxPop(60), //_maxPop(roundUp(60, _maxRobots)),
  _maxGenerations(2) {
  // Connection to server
  std::cout << "Connecting with VREP server. ";
  _clientID = simxStart("127.0.0.1", 19997, true, true, 5000, 5);
  if (_clientID == -1)
    throw "Can't connect to VREP server";
  std::cout << "Done." << std::endl;

  // Robots linkage
  std::cout << "Initializing robots. " << std::flush;
  #pragma omp parallel for
  for (uint8_t robotId = 0; robotId < _maxRobots; ++robotId) {
    auto robot = Robot(_clientID, robotId);
    #pragma omp critical
    {
      _robots.push_back(robot);
    }
  }
  std::cout << "Done." << std::endl;

  // Population creation
  std::cout << "Creating first population. ";
  _population = Population(_maxPop);
  std::cout << "Done." << std::endl;

  // Simulation overview
  std::cout << "Simulation overview:" << std::endl;
  std::cout << "- Robots:" << std::endl;
  for (auto robot : _robots) {
    std::cout << "\t robot " << (int)robot.getId() << " ready." << std::endl;
  }
  std::cout << "- Individuals:" << std::endl;
  _population.termDisplay();
}

/**
 *  Use a clock object ?
 *  Individual length and value limits are hardcoded :(
 */
int Simulation::run() {
  clock_t start = clock();
  std::cout << "## START" << std::endl;

  Individual best = _population.getElite(), worst = _population.getElite();
  for (int generationIter = 0; generationIter < _maxGenerations; ++generationIter) {
    // Displaying population global state
    std::cout << "Population size: " << _population.size() << std::endl;

    // Mutation
    _population.mutateBatch();

    // Processing each individual of the population
    for (uint i = 0; i < _population.size();) {
      // Retrieving batch of individuals to run simulation on
      population_t individualsBatch;
      for (int robotId = 0; (robotId < _maxRobots) && (i + robotId < _population.size()); ++robotId) {
        individualsBatch.push_back(_population[i + robotId]);
      }
      i += _maxRobots;

      // Start the simulation
      if (simxStartSimulation(_clientID, simx_opmode_oneshot_wait) == -1) {
        throw "Failed to start simulation";
      }

      // Each robot processing an individual in parallel
      #pragma omp parallel for
      for (uint batchIter = 0; batchIter < individualsBatch.size(); ++batchIter) {
        const auto &individual = individualsBatch[batchIter];
        const auto &robot = _robots[omp_get_thread_num()];

        robot.doActions(individual.getDna());
      }

      // Stop the simulation
      if (simxStopSimulation(_clientID, simx_opmode_oneshot_wait) == -1) {
        throw "Fail to stop simulation";
      }
      sleep(1);
    }

    // Evaluate the population
    _population.evaluateBatch();

    // Finding the worst of the current generation
    worst = _population.getWorst();
    std::cout << "Worst individual data: " << std::endl;
    worst.termDisplay();
    std::cout << "Fitness: " << worst.getScore() << std::endl;

    // Replacing worst of this generation by best of the previous one
    if (generationIter) {
      worst = best;
    }

    // Finding the best of the current generation
    best = _population.getElite();
    std::cout << "Best individual data: ";
    best.termDisplay();
    std::cout << "Fitness: " << best.getScore() << std::endl;

    // Selection for next generation
    breedingSeason();
  }

  std::cout << "## END" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC;
  return (0);
}

void
Simulation::breedingSeason() {
  // suming the weights for a weighted random
  fitness_t weightsSum = 0;
  // container for new generation
  Population newPopulationGeneration;
  unsigned int i;

  for (i = 0; i < _population.size(); i++)
    weightsSum += _population[i].getScore();

  // repeat mating as many time as needed to ensure constant population size
  #pragma omp parallel for
  for (i = 0; i < _population.size(); ++i) {
    const couple_t couple = makeCouple(weightsSum);
    const Individual child = crossOverSinglePoint(couple.first, couple.second).first;
    newPopulationGeneration.addIndividual(child);
  }
  _population = newPopulationGeneration;
}

couple_t Simulation::makeCouple(fitness_t weightsSum) {
  Individual leftMate, rightMate;
  unsigned int i;
  fitness_t randomWeightIndex;

  // repeating until we got two different individuals that can mate
  do {
    // get randomly a value corresponding to the weighted cursor
    randomWeightIndex = RandomGenerator::getInstance().d_between(0, weightsSum);

    // searching for the individual aimed by the cursor
    // (when the random weighted cursor goes under or is equal to 0)
    for (i = 0; i < _population.size(); i++) {
      const Individual &individual = _population[i];
      randomWeightIndex -= individual.getScore();
      if (randomWeightIndex <= 0) {
        if (leftMate.initialized())
          rightMate = individual;
        else
          leftMate = individual;
        break;
      }
    }
  } while (leftMate == rightMate || !leftMate.initialized() || !rightMate.initialized());
  // might create an infinite loop if all individual in the population are eaquals

  // returning the couple
  return {leftMate, rightMate};
}

couple_t Simulation::crossOverSinglePoint(const Individual &first, const Individual &second) {
  unsigned int length, size, i;
  Individual newFirst, newSecond;

  length = first.dnaSize() <= second.dnaSize() ? first.dnaSize() : second.dnaSize();
  size = RandomGenerator::getInstance().i_between(1, length);
  for (i = 0; i < size; i++) {
    newFirst.addGene(second.getGene(i));
    newSecond.addGene(first.getGene(i));
  }
  for (i = size; i < first.dnaSize(); i++)
    newFirst.addGene(first.getGene(i));
  for (i = size; i < second.dnaSize(); i++)
    newSecond.addGene(second.getGene(i));
  return {newFirst, newSecond};
}
