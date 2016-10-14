#include "Simulation.hh"

Simulation::Simulation()
//:  _connection(VrepConnection("127.0.0.1", 19997, 1, 1, 5000, 5))
{}

Simulation::~Simulation()
{}

/**
 *  Use a clock object ?
 *  Individual length and value limits are hardcoded :(
 */
void	Simulation::run() {
  Population _population = Population(maxPop);
  clock_t start = clock();

  std::cout << "## START" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC;

  for (uint16_t i = 0; i < maxTries; i++) {
    std::cout << "Population size: " << _population.getPopulation().size() << std::endl;
    // evaluate
    std::cout << "Global fitness: " << _population.evaluateBatch() << std::endl;
    // selection
    // elites
    // crossover
    // mutation

    // logs ?
    _population.termDisplay();
  }
  std::cout << "## END" << std::endl;
  std::cout << "## Time spent: " << double(clock() - start) / CLOCKS_PER_SEC;
}
