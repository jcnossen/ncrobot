#pragma once

#include "../Optimizer.h"


#define ST_STAR 0
#define ST_CYCLE 1
#define ST_KN 2
#define ST_SQUARE 3
#define ST_RANDOM 4

struct SwarmConfig
{
  SwarmConfig() { 
		popSize=20; 
		graphType=4;
		randomParam=0.1f; 	
    phi1=phi2=1.49618f;
    omega=0.7298f;
	}

  int popSize;
  int graphType; // ST_*
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
	void Update();
	void SetFitness(int particle, float fitness) { swarm[particle].fitness = fitness; }
	float* GetStateVector(int particle) { return &swarm[particle].position[0]; }
	int GetSize() { return swarm.size(); }

	static int numGraphTypes();
	static const char* graphTypeNames[];
};

