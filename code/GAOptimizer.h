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
#pragma  once

#include "Optimizer.h"

struct GAConfig
{
	int size;
	float pointMutationRate;
	float crossoverMutationRate;
};

struct GAGenome
{
	std::vector<float> genes;
	float fitness;

	void initialize(const std::vector<ParameterRange>& ranges);
};


/// Optimizer using genetic algorithms
class GAOptimizer : public Optimizer
{
public:
	GAOptimizer(GAConfig& cfg, std::vector<ParameterRange> ranges);
	~GAOptimizer();

	void Update();
	void SetFitness(int particle, float fitness);
	float* GetStateVector(int particle);
	int GetSize() { return config.size; }
private:
	std::vector<ParameterRange> ranges;
	typedef std::vector<GAGenome*> GenomeList;
	GenomeList genomes;

	GAConfig config;

	GAGenome* mutatedCopy(const std::vector<GAGenome*>& genomes);
};

