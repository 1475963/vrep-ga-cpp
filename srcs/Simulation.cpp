#include "Simulation.hh"

Simulation::Simulation()
//:  _connection(VrepConnection("127.0.0.1", 19997, 1, 1, 5000, 5))
{}

Simulation::~Simulation()
{}
/*

double  Simulation::evaluateIndividual(Individual individual) {
  return (0);
}

double  Simulation::evaluate(Population population) {
  return (0);
}
*/
/**
 * evaluate
 * selection
 * elites
 * crossover
 * mutation
 */
void	Simulation::run() {
  Population _population = Population();

  std::cout << _population.getPopulation().size() << std::endl;
  _population.termDisplay();

  _population.initialize(30);

  std::cout << _population.getPopulation().size() << std::endl;
  _population.termDisplay();
}
