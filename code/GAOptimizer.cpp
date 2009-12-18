/*
ncrobot
Natural Computing Robot
This app is based on the TestBed example application from Box2D.

Copyright 2009 Jelmer Cnossen, particle swarm optimization code by Johan de Ruiter
License (ZLIB):

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/

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

void GAOptimizer::Update()
{
	std::vector<GAGenome*> sortedList;
	sortedList.reserve(genomes.size());
	for(int i=0;i<genomes.size();i++)
		sortedList.push_back(genomes[i]);
	std::sort(sortedList.begin(), sortedList.end(), GenomeComparer);

	int numParents = sortedList.size()/5;
	sortedList.erase (sortedList.begin() + numParents, sortedList.end());

	std::vector<GAGenome*> newPopulation;

	// then fill up the remainder with mutated genomes
	for (int i=0;i<genomes.size();i++) {
		GAGenome* mutated = mutatedCopy(sortedList);
		newPopulation.push_back(mutated);
	}

	// delete previous generation
	DeleteAll(sortedList);
	
	// replace
	genomes = newPopulation;
}

GAGenome* GAOptimizer::mutatedCopy( const std::vector<GAGenome*>& genomes )
{
	// point mutations
	GAGenome *parent = genomes[rand() % genomes.size()];
	float pmRate = parent->genes[0];
	float pmSize = parent->genes[1];
	float crossoverRate = parent->genes[2];

//	d_trace("Parent %d has child. pmrate=%f, pmsize=%f\n", parentIndex, pmRate,pmSize);
	GAGenome *child = new GAGenome(*parent);

	for(int a=0;a<child->genes.size();a++) {
		if (randf() < pmRate)
			child->genes[a] += (randf() - 0.5f) * pmSize;
	}

	if (randf() < crossoverRate) {
		GAGenome* p2 = genomes[rand() % genomes.size()];
		int len = 2 + (int)(randf()*5+0.5f);
		int start = rand()%(p2->genes.size()-len);
		int dstPos = rand()%(child->genes.size()-len);

		for (int i=0;i<len;i++)
			child->genes[dstPos+i]=p2->genes[start+i];
	}
	return child;
}


void GAOptimizer::SetFitness( int particle, float fitness )
{
	genomes[particle]->fitness = fitness;
}

float* GAOptimizer::GetStateVector( int particle )
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
	genes[1] = 6.0f;
	genes[2] = 0.0001f;

	for(int i=0;i<ranges.size();i++) {
		genes[i+3] = ranges[i].min + (ranges[i].max-ranges[i].min) * randf();
	}
}

