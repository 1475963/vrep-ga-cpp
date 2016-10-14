#include "Simulation.hh"

Simulation::Simulation()
//:  _connection(VrepConnection("127.0.0.1", 19997, 1, 1, 5000, 5))
{}

Simulation::~Simulation()
{}

Population  *Simulation::generatePopulation(uint16_t maxPop) {
  Population *pop = new Population();

  for (auto i = 0; i < maxPop; i++) {
    Individual *individual = new Individual();

    for (auto a = 0; a < RandomGenerator::getInstance().i_between(0, 100); a++) {
      individual->_dna.push_back(RandomGenerator::getInstance().i_between(0, 10));
    }

    pop->push_back(individual);
  }

  return (pop);
}

double  Simulation::evaluateIndividual(Individual individual) {
  (void)individual;
  return (0);
}

double  Simulation::evaluate(Population population) {
  (void)population;
  return (0);
}

/**
 * evaluate
 * selection
 * elites
 * crossover
 * mutation
 */
void	Simulation::run() {
  Population *population = generatePopulation(200);

  for (unsigned int i = 0; i < population->size(); i++) {
    dna_t dna = population->at(i)->_dna;
    for (unsigned a = 0; a < dna.size(); a++) {
      std::cout << "Item N°" << i << ", action N°" << a << ": " << dna[a] << std::endl;
    }
  }
}
