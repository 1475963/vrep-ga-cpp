#include "Individual.hh"

Individual::Individual() {
    this->dna = dna_t();
    this->score = 0;
}

Individual::Individual(dna_t dna) {
    this->dna = dna;
}

Individual::~Individual() {

}