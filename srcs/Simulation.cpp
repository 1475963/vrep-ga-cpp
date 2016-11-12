#include "Simulation.hh"

static inline double round3d(double val) {
  return std::round(val * 1000.0) / 1000.0;
}

const std::map<const std::string, const Simulation::fp> Simulation::selections = {
  {"fitness-proportional", &Simulation::fitnessProportionalSelection},
  {"tournament", &Simulation::tournamentSelection}
};

const std::map<const std::string, const Simulation::func_ptr_t> Simulation::crossovers = {
  {"SinglePoint", &Simulation::crossOverSinglePoint},
  {"CutSplice", &Simulation::crossOverCutAndSplice},
  {"TowPoint", &Simulation::crossOverTwoPoint},
  {"Uniform", &Simulation::crossOverUniform}
};

Simulation::Simulation() :
  _maxRobots(omp_get_max_threads()),
  _maxPop(8),
  _maxGenerations(2) {
  _logger.push<std::string>("best\tworst\taverage", _globalLogFile);
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
  double	averageFitness;

  std::cout << "## START" << std::endl;
  for (int generationIter = 0; generationIter < _maxGenerations; ++generationIter) {
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
    averageFitness = _population.evaluateBatch();

    _population.sort();

    // Finding the worst of the current generation
    worst = _population[_population.size() - 1];

    // Finding the best of the current generation
    best = _population[0];

    logStats(best, worst, averageFitness);
    logPopulation(generationIter);

    // Replacing worst of this generation by best of this generation
    if (generationIter)
      worst = best;

    // container for new generation
    Population newPopulationGeneration;
    #pragma omp parallel for shared(newPopulationGeneration)
    for (unsigned int i = 0; i < _population.size(); i++) {
      couple_t selected = (this->*(selections.at("tournament")))();
      const Individual child = ((this->*crossovers.at("SinglePoint"))(selected.first,  selected.second)).first;
      newPopulationGeneration.addIndividual(child);
    }
   _population = newPopulationGeneration;

   // Mutation
    _population.mutateBatch();
  }

  std::cout << "## END" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC;
  return (0);
}

void	Simulation::logStats(const Individual &best, const Individual &worst, double avgFit) {
  _logger.push<double>(round3d(best.getScore()), _globalLogFile, '\t');
  _logger.push<double>(round3d(worst.getScore()), _globalLogFile, '\t');
  _logger.push<double>(avgFit, _globalLogFile);
}

void	Simulation::logPopulation(int index) {
  std::string filePath = "generation#" + std::to_string(index) + ".log";

  _logger.push<std::string>("Individual\tfitness", filePath);
  for (uint i = 0; i < _population.size(); i++) {
    const Individual &individual = _population[i];
    _logger.push(_population[i].getDna(), filePath, '\t');
    _logger.push<double>(individual.getScore(), filePath);
  }
  _logger.log(filePath);
}

/***** Selections *****/

couple_t	Simulation::fitnessProportionalSelection() const {
  Individual	leftMate, rightMate;
  unsigned int	i;
  fitness_t	randomWeightIndex, weightsSum = 0;

  for (i = 0; i < _population.size(); i++)
    weightsSum += _population[i].getScore();
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

couple_t		Simulation::tournamentSelection() const {
  int			i1 = -1, i2 = -1;
  uint			tournamentSize = 2, id;
  std::vector<uint>	ids;
  RandomGenerator	&rg = RandomGenerator::getInstance();

  do { // find 2 different individuals
    do { // make a tournament to find them
      id = rg.i_between(0, _population.size() - 1);
      if (std::find(ids.begin(), ids.end(), id) == ids.end())
  	ids.push_back(id);

    } while (ids.size() != tournamentSize);
    // keep the best individual for reproduction
    std::sort(ids.begin(), ids.end(), [](uint a, uint b) { return a > b; });
    if (i1 != -1)
      i2 = ids[0];
    else
      i1 = ids[0];
    ids.clear();
  } while (i1 == i2 || i1 == -1 || i2 == -1);
  return {_population[i1], _population[i2]};
}

/***** Crossovers *****/

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

couple_t Simulation::crossOverCutAndSplice(const Individual &first, const Individual &second) {
  unsigned int firstPoint = RandomGenerator::getInstance().i_between(0, first.dnaSize());
  unsigned int secondPoint = RandomGenerator::getInstance().i_between(0, second.dnaSize());
  Individual newFirst, newSecond;
  unsigned int i = 0;

  for (i = 0; i < firstPoint; i++)
    newFirst.addGene(first.getGene(i));
  for (i = 0; i < secondPoint; i++)
    newSecond.addGene(second.getGene(i));

  for (i = secondPoint; i < second.dnaSize(); i++)
    newFirst.addGene(second.getGene(i));
  for (i = firstPoint; i < first.dnaSize(); i++)
    newSecond.addGene(first.getGene(i));
  return {newFirst, newSecond};
}

couple_t Simulation::crossOverTwoPoint(const Individual &first, const Individual &second) {
  unsigned int length = first.dnaSize() <= second.dnaSize() ? first.dnaSize() : second.dnaSize();
  unsigned int firstPoint = RandomGenerator::getInstance().i_between(1, length-1);
  unsigned int secondPoint = RandomGenerator::getInstance().i_between(firstPoint, length);
  unsigned int i = 0;
  Individual newFirst, newSecond;

  for (i = 0; i < firstPoint; i++) {
    newFirst.addGene(first.getGene(i));
    newSecond.addGene(second.getGene(i));
  }
  for (i = firstPoint; i < secondPoint; i++) {
    newFirst.addGene(second.getGene(i));
    newSecond.addGene(first.getGene(i));
  }
  for (i = secondPoint; i < first.dnaSize(); i++) {
    newFirst.addGene(first.getGene(i));
  }
  for (i = secondPoint; i < second.dnaSize(); i++) {
    newSecond.addGene(second.getGene(i));
  }
  return {newFirst, newSecond};
}

couple_t Simulation::crossOverUniform(const Individual &first, const Individual &second) {
  unsigned int length = first.dnaSize() <= second.dnaSize() ? first.dnaSize() : second.dnaSize();
  Individual newFirst, newSecond;
  unsigned int i = 0;

  for (i = 0; i < length; i++) {
    if (RandomGenerator::getInstance().i_between(0, 100) < 50) {
      newFirst.addGene(first.getGene(i));
      newSecond.addGene(second.getGene(i));
    }
    else {
      newFirst.addGene(second.getGene(i));
      newSecond.addGene(first.getGene(i));
    }
  }

  for (i = length; i < first.dnaSize(); i++)
    newFirst.addGene(first.getGene(i));
  for (i = length; i < second.dnaSize(); i++)
    newSecond.addGene(second.getGene(i));
  return {newFirst, newSecond};
}
