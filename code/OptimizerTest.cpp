#include "StdIncl.h"
#include "Optimizer.h"
#include "GAOptimizer.h"
#include "swarm/swarm.h"


int DIM=30;
double pi=3.1415926535897;
  
float calcAckley(float *params) {
  float f=20+exp(1);

  float sumsq=0;
  for(int k=0;k<DIM;k++)
    sumsq+=params[k]*params[k];
  f+=-20*exp(-0.2*sqrt(sumsq/DIM));

  float sumcos=0;
  for(int k=0;k<DIM;k++)
    sumcos+=cos(2*pi*params[k]);
  f-=exp(sumcos/DIM);

  //f = 20 - 20 * exp(-0.2 * sqrt(sum(x .^ 2, 2) / N)) - exp(sum(cos(2 * pi * x), 2) / N) + exp(1);

  return f;
}


int main(int argc, char*argv[])
{
  int i,j,k;
  
  Optimizer* optimizer = 0;
  
  
  std::vector<ParameterRange> ranges(DIM);
  for(i=0;i<DIM;i++)
  {
    ranges[i].min=-50;
    ranges[i].max=50;
  }

  int popSize = 100;
  
  if (true) {
    SwarmConfig cfg;
    cfg.popSize = popSize;
    cfg.phi1=cfg.phi2=1.49618;
    cfg.omega=0.7298;

    Swarm *swarm = new Swarm(cfg, ranges);
    optimizer = swarm;
  }
  else {
    GAConfig cfg;

    cfg.size = popSize;
    cfg.pointMutationRate = 1.0f/DIM;
    cfg.crossoverMutationRate = 0.01f;
    GAOptimizer *ga= new GAOptimizer(cfg, ranges);
    optimizer = ga;
  }
    

  float BEST=-1e15;

  for(i=0;i<1000;i++) {
    optimizer->update();
    
    for (j=0;j<optimizer->getSize();j++) {
      float* params = optimizer->getStateVector(j);    
      
      float fitness = -calcAckley(params);

      if(fitness>BEST)
      {
        BEST=fitness;
        printf("Best=%.5f (iter %d)\n",BEST,i);
        for(k=0;k<DIM;k++) printf("(%.3lf)",params[k]);
        printf("\n\n");
      }
      
      optimizer->setFitness(j, fitness);
    }
  }  

  delete optimizer;

  return 0;
}




