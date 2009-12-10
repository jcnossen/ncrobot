#pragma once

#include "../Optimizer.h"

class Particle
{
public:
  Particle() { personalOptimum=fitness=0.0f;}
  float fitness,personalOptimum,friendOptimum;
  
  std::vector<float> position,velocity,personalBest,friendBest;
  std::vector<int> friends; 
};


class ParameterRange 
{
public:
  float min, max;
};



class Swarm : public Optimizer
{
public:
  int populationSize, dimension;
  std::vector<float> globalOptimal; float globalOptimum,phi1,phi2;
  std::vector<Particle> swarm;
	std::vector<ParameterRange> paramRanges;

	Swarm(SwarmConfig cfg, std::vector<ParameterRange> ranges);

  void setupGraph(int graphType, float randomParam);
  
	// Optimizer implementation
	void update();
	void setFitness(int particle, float fitness) { swarm[particle].fitness = fitness; }
	float* getStateVector(int particle) { return &swarm[particle].position[0]; }

	static int numGraphTypes();
	static const char* graphTypeNames[];
};

