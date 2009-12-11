#pragma once

#include "../Optimizer.h"

struct SwarmConfig
{
  SwarmConfig() { 
		popSize=10; 
		graphType=4; 
		randomParam=0.1; 	
    phi1=phi2=1.49618;
    omega=0.7298;
	}

  int popSize;
  int graphType;
  float randomParam;
  float phi1,phi2,omega;
};



class Particle
{
public:
  Particle() { personalOptimum=fitness=0.0f;}
  float fitness,personalOptimum,friendOptimum;
  
  std::vector<float> position,velocity,personalBest,friendBest;
  std::vector<int> friends; 
};



class Swarm : public Optimizer
{
public:
  int populationSize, dimension;
  std::vector<float> globalOptimal; float phi1,phi2,omega;
  std::vector<Particle> swarm;
	std::vector<ParameterRange> paramRanges;

	Swarm(SwarmConfig cfg, std::vector<ParameterRange> ranges);

  void setupGraph(int graphType, float randomParam);
  
	// Optimizer implementation
	void update();
	void setFitness(int particle, float fitness) { swarm[particle].fitness = fitness; }
	float* getStateVector(int particle) { return &swarm[particle].position[0]; }
	int getSize() { return swarm.size(); }

	static int numGraphTypes();
	static const char* graphTypeNames[];
};

