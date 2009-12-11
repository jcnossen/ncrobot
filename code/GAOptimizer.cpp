
#include "StdIncl.h"

#include "GAOptimizer.h"

static float randf() {
	return rand() / (float)RAND_MAX;
}

GAOptimizer::GAOptimizer( GAConfig& cfg, std::vector<ParameterRange> ranges )
{
	config = cfg;
	this->ranges = ranges;

	genomes.resize(cfg.size);
	for(int i=0;i<cfg.size ;i++) {
		genomes[i] = new GAGenome();
		genomes[i]->initialize(ranges);
	}
}

static bool GenomeComparer(GAGenome* a, GAGenome* b) {
	return a->fitness > b->fitness;
}

void GAOptimizer::update()
{
	// select the better half of the population, 
	std::vector<GAGenome*> sortedList;
	sortedList.reserve(genomes.size());
	for(int i=0;i<genomes.size();i++)
		sortedList.push_back(genomes[i]);
	std::sort(sortedList.begin(), sortedList.end(), GenomeComparer);

	std::vector<GAGenome*> newPopulation;

	int numParents = sortedList.size()/2;
	// and store half of it in the new population
	for (int i=0;i<numParents;i++)
		newPopulation.push_back(sortedList[i]);

	// then fill up the remainder with mutated genomes
	while (newPopulation.size() < config.size) {
		int parent = rand() % numParents;
		GAGenome* mutated = mutatedCopy(sortedList, parent);
		newPopulation.push_back(mutated);
	}

	// delete the genomes that did not have enough fitness
	for (int i=numParents; i < sortedList.size(); i++) {
		delete sortedList[i];
	}

	// replace
	genomes = newPopulation;
}

GAGenome* GAOptimizer::mutatedCopy( const std::vector<GAGenome*>& genomes, int parentIndex )
{
	// point mutations

	GAGenome *parent = genomes[parentIndex];
	float pmRate = parent->genes[0];
	float pmSize = parent->genes[1];
	float crossoverRate = parent->genes[2];

	d_trace("Parent %d has child. pmrate=%f, pmsize=%f\n", parentIndex, pmRate,pmSize);

	GAGenome *child = new GAGenome(*parent);

	for(int a=0;a<child->genes.size();a++) {
		if (randf() < pmRate)
			child->genes[a] += (randf() - 0.5f) * pmSize;
	}

	if (randf() < crossoverRate) {
	}
	return child;
}


void GAOptimizer::setFitness( int particle, float fitness )
{
	genomes[particle]->fitness = fitness;
}

float* GAOptimizer::getStateVector( int particle )
{
	return &genomes[particle]->genes.front();
}

GAOptimizer::~GAOptimizer()
{
	DeleteAll(genomes);
}

void GAGenome::initialize( const std::vector<ParameterRange>& ranges )
{
	genes.resize(ranges.size() + 3);

	// first 2 genes are point mutation rate, mutation size, and crossover rate
	genes[0] = 1.0f / genes.size();
	genes[1] = 2.0f;
	genes[2] = 0.0001f;

	for(int i=0;i<ranges.size();i++) {
		genes[i+3] = 0.0f;//ranges[i].min + (ranges[i].max-ranges[i].min) * randf();
	}
}

