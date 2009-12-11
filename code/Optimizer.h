#pragma once


class ParameterRange 
{
public:
	float min, max;
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

