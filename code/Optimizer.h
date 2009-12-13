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

	virtual void Update() = 0;
	virtual void SetFitness(int particle, float fitness) = 0;
	virtual float* GetStateVector(int particle) = 0;
	virtual int GetSize() =0 ;
};

