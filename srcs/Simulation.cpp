#include "Simulation.hh"

Simulation::Simulation() {
  // Connection to server
  std::cout << "Connecting with VREP server. ";
  _clientID = simxStart("127.0.0.1", 19997, true, true, 5000, 5);
  if (_clientID == -1)
    throw "Can't connect to VREP server";
  std::cout << "Done." << std::endl;

  // Robots linkage
  std::cout << "Initializing robots. " << std::flush;
  for (uint8_t robotId = 0; robotId < _maxRobots; ++robotId) {
    _robots.push_back(Robot(_clientID, robotId));
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
  std::cout << "run state: " << std::endl;
  _population.termDisplay();
  clock_t start = clock();

  std::cout << "## START" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC << std::endl;


  for (uint16_t i = 0; i < _maxTries; i++) {
    std::cout << "Population size: " << _population.size() << std::endl;
    // evaluate
    std::cout << "Global fitness: " << _population.evaluateBatch() << std::endl;

    // elites
    Individual &best = _population.getElite();
    std::cout << "Best individual data: " << std::endl;
    best.termDisplay();
    std::cout << "Fitness: " << best.getScore() << std::endl;

    // selection
    _population.termDisplay();
    breedingSeason();
    std::cout << "population after selection" << std::endl;
     _population.termDisplay();

    // mutation
    _population.mutateBatch();

    // replace worst by best
    Individual &worst = _population.getWorst();
    std::cout << "Worst individual data: " << std::endl;
    worst.termDisplay();
    std::cout << "Fitness: " << worst.getScore() << std::endl;
    worst = best;

    // for each individual start a simulation and execute the dna on vrep
    for (unsigned int i = 0; i < _population.size();) {
      simxInt ret = simxStartSimulation(_clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStartSimulation error: " << ret << std::endl;
        return ret;
      }

      // parallelize here
      for (uint j = 0; j < _maxRobots && i < _population.size(); ++j, ++i) {
        const Individual &individual = _population[i];

        individual.termDisplay();
        ret = _robots[j].doActions(individual.getDna());
        if (ret != 0) {
          std::cerr << "Robot::doActions error: " << ret << std::endl;
        //        return ret;
        }
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
  // container for new generation
  Population newPopulationGeneration;
  unsigned int i;

  for (i = 0; i < _population.size(); i++)
    weightsSum += _population[i].getScore();

  // repeat mating as many time as needed to ensure constant population size
  for (i = 0; i < _population.size(); ++i) {
    const couple_t couple = makeCouple(weightsSum);
    const Individual child = ( (this->*crossovers.at("Uniform"))(couple.first, couple.second) ).first;
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

const std::map<const std::string, const Simulation::func_ptr_t> Simulation::crossovers = {
  {"SinglePoint", &Simulation::crossOverSinglePoint},
  {"Uniform", &Simulation::crossOverUniform}
};


couple_t Simulation::crossOverUniform(const Individual &first, const Individual &second) {
  int length = first.dnaSize() <= second.dnaSize() ? first.dnaSize() : second.dnaSize();
  Individual newFirst, newSecond;
  
  for (int i = 0; i < length; i++) {
    if (RandomGenerator::getInstance().i_between(0, 100) < 50)
      newFirst.addGene(first.getGene(i));
    else
      newFirst.addGene(second.getGene(i));
    if (RandomGenerator::getInstance().i_between(0, 100) < 50)
      newSecond.addGene(first.getGene(i));
    else
      newSecond.addGene(second.getGene(i));
  }
  for (unsigned int i = length; i < first.dnaSize(); i++)
    newFirst.addGene(first.getGene(i));
  for (unsigned int i = length; i < second.dnaSize(); i++)
    newSecond.addGene(second.getGene(i));
  std::cout << "ADN UNIFORM" << std::endl;
  newFirst.termDisplay();
  newSecond.termDisplay();
  std::cout << "END" << std::endl;

  return {newFirst, newSecond};
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
