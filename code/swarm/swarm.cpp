#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <stack>
#include <set>
#include <map>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <list>
#include <numeric>
#include <algorithm>

#include "swarm.h"
using namespace std;


float Normal(double mean, double stdev)
{
  float U1=((rand()%32000)+1)/32000.0;
  float U2=((rand()%32000)+1)/32000.0;

  float Z=sqrt(-2*log(U1))*cos(2*3.14159265358979*U2);

  return Z*stdev+mean;
}

float Uniform(float low, float high)
{
  return ((rand()%32000)/32000.0)*(high-low)+low;
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

  if(graphType==0)
    for(i=1;i<populationSize;i++)
    {
      swarm[0].friends.push_back(i);
      swarm[i].friends.push_back(0);
    }

  if(graphType==1)
    for(i=0;i<populationSize;i++)
    {
      swarm[i].friends.push_back((i+1)%populationSize);
      swarm[(i+1)%populationSize].friends.push_back(i);
    }   

  if(graphType==2)
    for(i=0;i<populationSize;i++)
      for(j=0;j<populationSize;j++)
        if(j!=i)
          swarm[i].friends.push_back(j);
          
  if(graphType==3)
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

  if(graphType==4)
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



Swarm::Swarm(SwarmConfig cfg)
{
	this->populationSize = cfg.popSize;
	this->paramRanges = cfg.paramRanges;
	this->dimension = cfg.paramRanges.size();
	this->phi1=cfg.phi1;
	this->phi2=cfg.phi2;

//printf("dim=%d\n",dimension);
	int i,j;
  globalOptimum=0; 
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
    
    for(i=0;i<dimension;i++) 
      swarm[j].position[i]=((rand()%1000000)/1000000.0)*(paramRanges[i].max-paramRanges[i].min)+paramRanges[i].min;
  }

//printf("hier\n");
	setupGraph(cfg.graphType,cfg.randomParam); 
}







//This function assumes the new fitnessvalues have just been updated
void Swarm::update()
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
      swarm[j].velocity[i]+=Uniform(0,phi1)*(swarm[j].personalBest[i]-swarm[j].position[i]);
      swarm[j].velocity[i]+=Uniform(0,phi2)*(swarm[j].friendBest[i]-swarm[j].position[i]);    
    }

    //update position  
    for(i=0;i<dimension;i++)
      swarm[j].position[i]=max(paramRanges[i].min,min(paramRanges[i].max,swarm[j].position[i]+swarm[j].velocity[i]));
  }
}





