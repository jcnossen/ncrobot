#pragma once

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


struct SwarmConfig
{
  SwarmConfig() { popSize=10; graphType=4; randomParam=0.1; phi1=2; phi2=2; }

  int popSize;
  int graphType;
  float randomParam;
  std::vector<ParameterRange> paramRanges;
  float phi1,phi2;
};

class Swarm
{
public:
  int populationSize, dimension;
  std::vector<float> globalOptimal; float globalOptimum,phi1,phi2;
  std::vector<Particle> swarm;
	std::vector<ParameterRange> paramRanges;

	Swarm(SwarmConfig cfg);

  void setupGraph(int graphType, float randomParam);
  void update();

	static int numGraphTypes();
	static const char* graphTypeNames[];

};


double normaldistributionvariable(double mean, double stdev);

