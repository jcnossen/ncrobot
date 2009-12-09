#pragma once

class Particle
{
  public:

  float fitness;
  std::vector<float> position;
  std::vector<float> velocity;
  std::vector<int> friends;
};


class ParameterRange 
{
  public:
  float min, max;
};


class Swarm
{
  public:
  int populationSize, dimension;
  std::vector<Particle> swarm;
	std::vector<ParameterRange> paramRanges;

  void readInput();
  void initialize();
  void update();
};


double normaldistributionvariable(double mean, double stdev);

