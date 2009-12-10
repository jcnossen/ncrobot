#pragma  once

#include "Optimizer.h"

/// Optimizer using genetic algorithms
class GAOptimizer : public Optimizer
{
public:
	GAOptimizer();

	void update();
	void setFitness(int particle, float fitness);
	float* getStateVector(int particle);
	
};

