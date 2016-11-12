#include "Simulation.hh"

static inline double round3d(double val) {
  return std::round(val * 1000.0) / 1000.0;
}

Simulation::Simulation() :
  _maxRobots(omp_get_max_threads()),
  _maxPop(16),
  _maxGenerations(5) {
  _logger.push<std::string>("best\tworst\taverage", _globalLogFile);
  // Connection to server
  std::cout << "Connecting with VREP server. ";
  _clientID = simxStart("127.0.0.1", 19997, true, true, 5000, 5);
  if (_clientID == -1)
    throw "Can't connect to VREP server";
  std::cout << "Done." << std::endl;

  auto err = simxGetObjectHandle(
    _clientID,
    "ResizableFloot_5_25",
    &_floorHandler,
    simx_opmode_blocking
  );

  if (err == -1) {
    throw "Can't get object handle for floor";
  }

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

Simulation::~Simulation() {
  _logger.log(_globalLogFile);
}

/**
 *  Use a clock object ?
 *  Individual length and value limits are hardcoded :(
 */
int		Simulation::run() {
  clock_t	start = clock();
  Individual	best, worst;
  double	averageFitness, bestFit, worstFit;

  std::cout << "## START" << std::endl;
  for (int generationIter = 0; generationIter < _maxGenerations; ++generationIter) {
    _logger.push<std::string>("Individual\tfitness", "generation#" + std::to_string(generationIter) + ".log");

    // Processing each individual of the population
    for (uint i = 0; i < _population.size();) {
      // Retrieving batch of individuals to run simulation on
      population_t individualsBatch;
      for (int robotId = 0; (robotId < _maxRobots) && (i + robotId < _population.size()); ++robotId) {
        individualsBatch.push_back(_population.at(i + robotId));
      }

      // Start the simulation
      if (simxStartSimulation(_clientID, simx_opmode_oneshot_wait) == -1) {
        throw "Failed to start simulation";
      }

      // Each robot processing an individual in parallel
      #pragma omp parallel for
      for (uint batchIter = 0; batchIter < individualsBatch.size(); ++batchIter) {
        auto &individual = individualsBatch.at(batchIter);
        const auto &robot = _robots[omp_get_thread_num()];

        simxFloat prevPos[3];
        robot.getPosition(prevPos, _floorHandler);

        std::cout << "DNA: " << std::endl;
        individual.termDisplay();
        std::cout << "Start X: " << prevPos[0] << ", Y: " << prevPos[1] << std::endl;

        robot.doActions(individual.getDna());

        simxFloat nextPos[3];
        robot.getPosition(nextPos, _floorHandler);

        std::cout << "End X: " << nextPos[0] << ", Y: " << nextPos[1] << std::endl;

        double fitness = individual.evaluate(prevPos, nextPos);
        std::cout << "Fitness computed: " << fitness << std::endl;
        _population[i + batchIter].setScore(individual.getScore());
      }

      i += _maxRobots;

      // Stop the simulation
      if (simxStopSimulation(_clientID, simx_opmode_oneshot_wait) == -1) {
        throw "Fail to stop simulation";
      }
    }

    _population.termDisplay();

    if (generationIter > 0) {
      // Finding the worst of the current generation
      worst = _population.getWorst();
      // Replacing worst of this generation by best of the previous one
      worst = best;
      std::cout << "Worst replaced" << std::endl;
    }

    // Finding the best of the current generation
    best = _population.getElite();

    _logger.push<double>(round3d(best.getScore()), _globalLogFile, '\t');
    _logger.push<double>(round3d(worst.getScore()), _globalLogFile, '\t');
    _logger.push<double>(averageFitness, _globalLogFile);
    logPopulation(generationIter);

    // Selection for next generation
    breedingSeason();
//    selection();
    std::cout << "Selection done" << std::endl;
    // Mutation
    _population.mutateBatch();
  }

  std::cout << "## END" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC;
  return (0);
}

void	Simulation::logPopulation(int index) {
  std::string filePath = "generation#" + std::to_string(index) + ".log";

  for (uint i = 0; i < _population.size(); i++) {
    const Individual &individual = _population.at(i);
    _logger.push(individual.getDna(), filePath, '\t');
    _logger.push<double>(individual.getScore(), filePath);
  }
  _logger.log(filePath);
}

void Simulation::selection() {
  Population newPop;

  for (uint i = 0; i < _population.size(); ++i) {
    uint index = rouletteWheel();
    Individual indiv = _population.at(index);
    // crossover rate
    if (RandomGenerator::getInstance().d_between(0, 1) <= 0.7f) {
      index = rouletteWheel();
      // at random, should launch a simulation to compute fitness and keep the best
      if (RandomGenerator::getInstance().d_between(0, 1) <= 0.5f) {
        indiv = crossOverSinglePoint(indiv, _population.at(index)).first;
      }
      else {
        indiv = crossOverSinglePoint(indiv, _population.at(index)).second;
      }
    }
    newPop.addIndividual(indiv);
  }

  _population = newPop;
}

uint Simulation::rouletteWheel() {
  double maxFitness = 0, sumFitness = 0;
  uint i = 0;

  for (i = 0; i < _population.size(); ++i) {
    maxFitness += _population.at(i).getScore();
  }

  double target = RandomGenerator::getInstance().d_between(0, maxFitness);

  for (i = 0; sumFitness < target; ++i) {
    sumFitness += _population.at(i).getScore();
  }

  return i;
}

void
Simulation::breedingSeason() {
  // suming the weights for a weighted random
  fitness_t weightsSum = 0;
  // container for new generation
  Population newPopulationGeneration;
  unsigned int i;

  for (i = 0; i < _population.size(); i++)
    weightsSum += _population.at(i).getScore();

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
      const Individual &individual = _population.at(i);
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
