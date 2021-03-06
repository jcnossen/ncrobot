#include "../StdIncl.h"
#include "swarm.h"
using namespace std;

const char* Swarm::graphTypeNames[] = {

  "star topology",
  "cycle topology",
  "Kn topology",
  "square grid topology",
  "random graph"
};

int Swarm::numGraphTypes() { return sizeof(graphTypeNames) / sizeof(const char*); }


float Normal(double mean, double stdev)
{
  float U1=rand()/(float)RAND_MAX;
  float U2=rand()/(float)RAND_MAX;

  float Z=sqrt(-2*log(U1))*cos(2*3.14159265358979*U2);

  return Z*stdev+mean;
}

float Uniform(float low, float high)
{
  return (rand()/(float)RAND_MAX)*(high-low)+low;
}



//Create friendship network graph.
//graphType values: 0 -> star topology
//                  1 -> cycle topology
//                  2 -> Kn topology
//                  3 -> square grid topology       
//                  4 -> random graph
void Swarm::setupGraph(int graphType, float randomParam)
{
  int i,j;

  if(graphType==ST_STAR)
    for(i=1;i<populationSize;i++)
    {
      swarm[0].friends.push_back(i);
      swarm[i].friends.push_back(0);
    }

  if(graphType==ST_CYCLE)
    for(i=0;i<populationSize;i++)
    {
      swarm[i].friends.push_back((i+1)%populationSize);
      swarm[(i+1)%populationSize].friends.push_back(i);
    }   

  if(graphType==ST_KN)
    for(i=0;i<populationSize;i++)
      for(j=0;j<populationSize;j++)
        if(j!=i)
          swarm[i].friends.push_back(j);
          
  if(graphType==ST_SQUARE)
  {
    int z = (int)sqrtf(populationSize);
    if(z*z!=populationSize) z++;
  
    //horizontal edges
    for(j=0;j<z;j++)
      for(i=0;i<z-1;i++)
        if(j*z+i+1<populationSize)
        {
          swarm[j*z+i].friends.push_back(j*z+i+1);
          swarm[j*z+i+1].friends.push_back(j*z+i);
        }
  
    //vertical edges
    for(i=0;i<z;i++)
      for(j=0;j<z-1;j++)
        if((j+1)*z+i<populationSize)
        {
          swarm[j*z+i].friends.push_back((j+1)*z+i);
          swarm[(j+1)*z+i].friends.push_back(j*z+i);
        }
  }

  if(graphType==ST_RANDOM)
  {
    for(i=0;i<populationSize;i++)
      for(j=i+1;j<populationSize;j++)
        if(rand()%1000<1000*randomParam)
        {
          swarm[j].friends.push_back(i);
          swarm[i].friends.push_back(j);        
        }
  }

}



Swarm::Swarm(SwarmConfig cfg, std::vector<ParameterRange> ranges)
{
	this->populationSize = cfg.popSize;
	this->paramRanges = ranges;
	this->dimension = ranges.size();
	this->phi1=cfg.phi1;
	this->phi2=cfg.phi2;
	this->omega=cfg.omega;

//printf("dim=%d\n",dimension);
	int i,j;

	swarm.resize(populationSize);
//printf("swarmsize=%d\n",(int)swarm.size());
  for(j=0;j<populationSize;j++)
  {
    swarm[j].personalOptimum=0;
    swarm[j].friendOptimum=0;
    swarm[j].position=vector<float>(dimension,0.0);
    swarm[j].velocity=vector<float>(dimension,0.0);
    swarm[j].personalBest=vector<float>(dimension,0.0);
    swarm[j].friendBest=vector<float>(dimension,0.0);
    
		for(i=0;i<dimension;i++) {
			ParameterRange& r= paramRanges[i];
      swarm[j].position[i] = Uniform(r.min, r.max);
		}
  }

//printf("hier\n");
	setupGraph(cfg.graphType,cfg.randomParam); 
}







//This function assumes the new fitnessvalues have just been updated
void Swarm::Update()
{
  int i,j,k;

  //First update the personalBest and friendBest
  for(j=0;j<populationSize;j++)
  {
    //if there is an improvement...
    if(swarm[j].fitness>swarm[j].personalOptimum)
    {
      //update the personal best
      swarm[j].personalOptimum=swarm[j].fitness;
      swarm[j].personalBest=swarm[j].position;
      
      //perhaps also update the friends best
      if(swarm[j].fitness>swarm[j].friendOptimum)
      {
        swarm[j].friendOptimum=swarm[j].fitness;
        swarm[j].friendBest=swarm[j].position;
      }
      
      //Broadcast this result to friends
      for(k=0;k<(int)swarm[j].friends.size();k++)
      {      
        int f=swarm[j].friends[k];
        if(swarm[j].fitness>swarm[f].friendOptimum)
        {
          swarm[f].friendOptimum=swarm[j].fitness;
          swarm[f].friendBest=swarm[j].position;
        }
      }
    }
  }



  //Then adjust the position and velocity vectors
  for(j=0;j<populationSize;j++)
  {
    //update velocity  
    for(i=0;i<dimension;i++)
    {
      swarm[j].velocity[i]*=omega;
      swarm[j].velocity[i]+=Uniform(0,phi1)*(swarm[j].personalBest[i]-swarm[j].position[i]);
      swarm[j].velocity[i]+=Uniform(0,phi2)*(swarm[j].friendBest[i]-swarm[j].position[i]);    
    }

    //update position  
    for(i=0;i<dimension;i++)
      swarm[j].position[i]=max(paramRanges[i].min,min(paramRanges[i].max,swarm[j].position[i]+swarm[j].velocity[i]));
  }
}





