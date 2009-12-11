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

	void update();
	void setFitness(int particle, float fitness);
	float* getStateVector(int particle);
	int getSize() { return config.size; }
private:
	std::vector<ParameterRange> ranges;
	typedef std::vector<GAGenome*> GenomeList;
	GenomeList genomes;

	GAConfig config;

	GAGenome* mutatedCopy(const std::vector<GAGenome*>& genomes, int parent);
};

