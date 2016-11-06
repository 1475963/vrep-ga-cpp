#include "Simulation.hh"

Simulation::Simulation() :
  _population(Population(_maxPop)),
  _clientID(simxStart("127.0.0.1", 19997, true, true, 5000, 5)),
  _robot(Robot(_clientID)) {
  if (_clientID == -1) {
    throw "simxStart error";
  }
}

/**
 *  Use a clock object ?
 *  Individual length and value limits are hardcoded :(
 */
int Simulation::run() {
  clock_t start = clock();

  std::cout << "## START" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC << std::endl;


  for (uint16_t i = 0; i < _maxTries; i++) {
    std::cout << "Population size: " << _population.size() << std::endl;
    // evaluate
    std::cout << "Global fitness: " << _population.evaluateBatch() << std::endl;

    // selection
    _population.termDisplay();
    breedingSeason();
    std::cout << "population after selection" << std::endl;
     _population.termDisplay();

    // elites
    Individual &best = _population.getElite();
    std::cout << "Best individual data: " << std::endl;
    best.termDisplay();
    std::cout << "Fitness: " << best.getScore() << std::endl;

    Individual &worst = _population.getWorst();
    std::cout << "Worst individual data: " << std::endl;
    worst.termDisplay();
    std::cout << "Fitness: " << worst.getScore() << std::endl;

    // crossover
    crossOverSinglePoint(best, worst);

    // mutation
    _population.mutateBatch();

    // for each individual start a simulation and execute the dna on vrep
    for (unsigned int i = 0; i < _population.size(); i++) {
      const Individual &individual = _population[i];
      simxInt ret  = simxStartSimulation(_clientID, simx_opmode_oneshot_wait);

      if (ret != 0) {
        std::cerr << "simxStartSimulation error: " << ret << std::endl;
        return ret;
      }

      individual.termDisplay();
      ret = _robot.doActions(individual.getDna());
      if (ret != 0) {
        std::cerr << "Robot::doActions error: " << ret << std::endl;
//        return ret;
      }

      ret = simxStopSimulation(_clientID, simx_opmode_oneshot_wait);
      if (ret != 0) {
        std::cerr << "simxStopSimulation error: " << ret << std::endl;
        return ret;
      }
      sleep(2);
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
    const Individual child = crossOverSinglePoint(couple.first, couple.second).first;
    newPopulationGeneration.addChild(child);
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
