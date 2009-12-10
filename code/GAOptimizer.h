#pragma  once

#include "Optimizer.h"

struct GAConfig
{
	int size;
	float pointMutationRate;
	float crossoverMutationRate;
};

/// Optimizer using genetic algorithms
class GAOptimizer : public Optimizer
{
public:
	GAOptimizer(GAConfig& cfg, std::vector<ParameterRange> ranges);

	void update();
	void setFitness(int particle, float fitness);
	float* getStateVector(int particle);
};

