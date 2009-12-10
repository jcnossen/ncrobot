#pragma once


// abstract optimizer class
class Optimizer
{
public:
	virtual void update() = 0;
	virtual void setFitness(int particle, float fitness) = 0;
	virtual float* getStateVector(int particle) = 0;
};