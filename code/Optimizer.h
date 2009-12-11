#pragma once


class ParameterRange 
{
public:
	float min, max;
};


struct SwarmConfig
{
  SwarmConfig() { popSize=10; graphType=4; randomParam=0.1; phi1=0.2; phi2=0.2; }

  int popSize;
  int graphType;
  float randomParam;
  float phi1,phi2;
};


// abstract optimizer class
class Optimizer
{
public:
	virtual ~Optimizer() {}

	virtual void update() = 0;
	virtual void setFitness(int particle, float fitness) = 0;
	virtual float* getStateVector(int particle) = 0;
	virtual int getSize() =0 ;
};

