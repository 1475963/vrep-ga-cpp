#include "Simulation.hh"

Simulation::Simulation()
: _population(Population(_maxPop)),
  _clientID(simxStart("127.0.0.1", 19997, true, true, 5000, 5))
//  _robot(Robot(_clientID))
//  _robots(std::vector<Robot>(0))
//:  _connection(VrepConnection("127.0.0.1", 19997, 1, 1, 5000, 5))
{
  if (_clientID == -1) {
    throw "simxStart error";
  }
  for (uint16_t i = 0; i < _maxRobots; ++i) {
    _robots.push_back(Robot(_clientID));
    usleep(100);
  }
  std::cout << "init state: " << std::endl;
  _population.termDisplay();
}

Simulation::~Simulation()
{}

/**
 *  Use a clock object ?
 *  Individual length and value limits are hardcoded :(
 */
int Simulation::run() {
  std::cout << "run state: " << std::endl;
  _population.termDisplay();
  clock_t start = clock();
  exit(0);

  std::cout << "## START" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC << std::endl;


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
    Individual *best = _population.getElite();
    std::cout << "Best individual data: " << std::endl;
    best->termDisplay();
    std::cout << "Fitness: " << best->getScore() << std::endl;

    Individual *worst = _population.getWorst();
    std::cout << "Worst individual data: " << std::endl;
    worst->termDisplay();
    std::cout << "Fitness: " << worst->getScore() << std::endl;

    // crossover
    crossOverSinglePoint(best, worst);

    // mutation
    _population.mutateBatch();

    // for each individual start a simulation and execute the dna on vrep
    for (uint16_t i = 0; i < _population.getPopulation().size(); ++i) {
      Individual *individual = _population.getPopulation().at(i);
      simxInt ret;
      ret = simxStartSimulation(_clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStartSimulation error: " << ret << std::endl;
        return ret;
      }

      individual->termDisplay();
      ret = _robots[i % _maxRobots].doActions(individual->getDna());
      if (ret != 0) {
        std::cerr << "Robot::doActions error: " << ret << std::endl;
//        return ret;
      }

      ret = simxStopSimulation(_clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStopSimulation error: " << ret << std::endl;
        return ret;
      }
      sleep(1);
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
    auto child = crossOverSinglePoint(couple.first, couple.second).first;
    newPopulationGeneration.addChild(child);
  }
  _population = newPopulationGeneration;
}

couple_t
Simulation::makeCouple(fitness_t weightsSum) {
  Individual *leftMate = nullptr, *rightMate = nullptr;

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

std::pair<Individual*, Individual*> Simulation::crossOverSinglePoint(Individual *first, Individual *second) {
  int size;
  int length;

  length = (first->getDna().size() <= second->getDna().size()) ? first->getDna().size() : second->getDna().size();
  size = RandomGenerator::getInstance().i_between(0, length);

  Individual *newFirst = new Individual(0);
  Individual *newSecond = new Individual(0);

  for (int i = 0; i < size; i++) {
    newFirst->getDna().push_back(second->getDna().at(i));
    newSecond->getDna().push_back(first->getDna().at(i));
  }
  for (unsigned int i = size; i < first->getDna().size(); i++) {
    newFirst->getDna().push_back(first->getDna().at(i));
  }
  for (unsigned int i = size; i < second->getDna().size(); i++) {
    newSecond->getDna().push_back(second->getDna().at(i));
  }

  return std::pair<Individual *, Individual *>(newFirst, newSecond);
}
