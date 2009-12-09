#include <string>
#include <cstdio>
#include <vector>
#include "swarm.h"
using namespace std;

int main()
{
  vector<ParameterRange> ranges(15);
  for(int i=0;i<15;i++) { ranges[i].min=0; ranges[i].max=180; }
  
  SwarmConfig cfg;
  cfg.paramRanges=ranges;
  
  Swarm myswarm(cfg);

  

  return 0;
}


