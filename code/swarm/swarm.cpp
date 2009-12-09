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


double normaldistribution(double mean, double stdev)
{
  double U1=((rand()%1000000)+1)/1000000.0;
  double U2=((rand()%1000000)+1)/1000000.0;

  double Z=sqrt(-2*log(U1))*cos(2*3.14159265358979*U2);

  return Z*stdev+mean;
}


Swarm::Swarm(int popSize, int graphType, float randomParam, std::vector<ParameterRange> ranges)
{
	this->populationSize = popSize;
	this->paramRanges = paramRanges;
	this->dimension = paramRanges.size();

	swarm.resize(popSize);
	for(int i=0;i<swarm.size();i++) {
		swarm[i].position.resize(ranges.size());
		swarm[i].velocity.resize(ranges.size());
	}
}



//This function takes information about how to set up a swarm from stdin
void Swarm::readInput()
{
  int i,j,k;
  scanf("%d",&populationSize);
  

  swarm=vector<Particle>(populationSize);
  
  int friendships;
  scanf("%d",&friendships);

  //A 'friendships' value >=0 indicates one can expect this number of unidirectional edges: from to
  for(i=0;i<friendships;i++)
  {
    scanf("%d %d",&j,&k);
    swarm[j].friends.push_back(k);
  }

  //negative 'friendships' values: -1 -> star topology
  //                               -2 -> cycle topology
  //                               -3 -> Kn topology
  //                               -4 -> square grid topology       
  //                               -5 -> random graph
  
  
  if(friendships==-1)
    for(i=1;i<populationSize;i++)
    {
      swarm[0].friends.push_back(i);
      swarm[i].friends.push_back(0);
    }

  if(friendships==-2)
    for(i=0;i<populationSize;i++)
    {
      swarm[i].friends.push_back((i+1)%populationSize);
      swarm[(i+1)%populationSize].friends.push_back(i);
    }   

  if(friendships==-3)
    for(i=0;i<populationSize;i++)
      for(j=0;j<populationSize;j++)
        if(j!=i)
          swarm[i].friends.push_back(j);
          
  if(friendships==-4)
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

  if(friendships==-5)
  {
    double density;
    scanf("%lf",&density);
    for(i=0;i<populationSize;i++)
      for(j=i+1;j<populationSize;j++)
        if(rand()%1000<1000*density)
        {
          swarm[j].friends.push_back(i);
          swarm[i].friends.push_back(j);        
        }
  }


  return;
}


void Swarm::initialize()
{
  int i,j;
  dimension=paramRanges.size();
  globalOptimum=1e15; //aangenomen dat we minimaliseren!!!!
  for(j=0;j<populationSize;j++)
  {
    swarm[j].localOptimum=1e15;
    swarm[j].velocity=vector<float>(dimension,0.0);
    for(i=0;i<dimension;i++)
      swarm[j].position.push_back(((rand()%1000000)/1000000.0)*(paramRanges[i].max-paramRanges[i].min)+paramRanges[i].min);
  }
}


//This function assumes the new fitnessvalues have just been updated
void Swarm::update()
{
  int i,j,k;



  //update velocities




  //update positions
  for(j=0;j<populationSize;j++)
    for(i=0;i<dimension;i++)
      swarm[j].position[i]+=swarm[j].velocity[i];
}





