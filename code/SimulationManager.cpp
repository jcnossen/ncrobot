
#include "StdIncl.h"
#include "SimulationManager.h"

#include "Test.h"

#include <SDL.h>
#include <SDL_thread.h>

#include "ThreadManager.h"

SimulationManager::SimulationManager()
{
	optimizer = 0;
	testFactory	= 0;
	stopOptimization = false;
	isOptimizing = false;
	test = 0;
	tick = 0;
	bestRun = 0;
	hashFailed = false;
}

SimulationManager::~SimulationManager()
{
	SafeDelete(test);
	SafeDelete(optimizer);
}



struct SimulationWorkItem
{
	SimulationManager* owner;
	Test* test;
	float score;
	int index;
	int simTicks;
	std::vector<uint> hashHistory;
	TestSettings* settings;
};

void RunTest_WorkItem(void* d)
{
	SimulationWorkItem* p = (SimulationWorkItem*)d;

	// run simulation, and feed scores back into swarm
	p->test->SetupForPSO();
	p->hashHistory.push_back(p->test->CalcHash());

	for(int u=0;u<p->simTicks;u++) {
		p->test->Step(p->settings);
		p->hashHistory.push_back(p->test->CalcHash());
	}

	p->score = p->test->GetScore();
}


void SimulationManager::Optimize()
{
	Uint32 startTicks = SDL_GetTicks();
	int simticks = optimizeSettings.hz * config.simLength;
	d_trace("Starting optimization with %d threads. Simticks=%d\n", config.numSimThreads, simticks);

	std::vector<ParameterRange> ranges = GetRanges();
	bestRun = 0;

	int i = 0;
	while (!stopOptimization) {
		float t = (SDL_GetTicks()-startTicks) * 0.001f;
		if (t >= config.maxTime || i == config.maxTicks)
			break;

		d_trace("Swarm update tick: %d. T (minutes)=%f. ", i, t/60.0f);

		optimizer->Update();

		ThreadManager threadManager;
		std::vector<SimulationWorkItem> workItemParams(config.population);

		// post items for the simulation threads
		for(int j=0;j<config.population;j++) {
			SimulationWorkItem& wi = workItemParams[j];
			wi.test = testFactory->createFcn(); 
			wi.owner = this;
			wi.simTicks = simticks;
			wi.settings = &optimizeSettings;
			wi.test->SetControlParams(optimizer->GetStateVector(j));
			threadManager.AddWorkItem(RunTest_WorkItem, &workItemParams[j]);
		}
		threadManager.Start(config.numSimThreads);
		threadManager.WaitForAllDone();

		// Update best score and create run history
		for(int j=0;j<config.population;j++) {
			SimulationWorkItem& workItem = workItemParams[j];

			optimizer->SetFitness(j, workItem.score);
			float* sv = optimizer->GetStateVector(j);
			runHistory.push_back(RunInfo());
			RunInfo& ri = runHistory.back();
			ri.score = workItem.score;
			ri.controlState = std::vector<float> (sv, sv + ranges.size());
			ri.hashHistory = workItem.hashHistory;
			SafeDelete(workItem.test);

			if (!bestRun || workItem.score > bestRun->score)
				bestRun = &runHistory.back();
		}

		d_trace("BestScore: %f\n", bestRun ? bestRun->score : 0.0f);
		i++;
	}


	if (bestRun) {
		for(int a=0;a<bestRun->controlState.size();a++)
			d_trace("Param[%d]: %f\n", a, bestRun->controlState[a]);
		d_trace("Hash: %X\n", bestRun->hashHistory.back());

		Reset();
	}

	SafeDelete(optimizer);
	isOptimizing = false;
}




void SimulationManager::SetOptimizer( Optimizer* o )
{
	if (optimizer)
		delete optimizer;
	optimizer = o;
}

int SimulationManager::OptimizeThreadMain(void *p)
{
	SimulationManager* sm = (SimulationManager*)p;
	sm->Optimize();
	return 0;
}


void SimulationManager::StartOptimization( SimulationConfig cfg )
{
	config = cfg;
	runHistory.clear();
	bestRun = 0;
	isOptimizing=true;
	stopOptimization = false;

	optimizeSettings = interactiveSettings;
	optimizeSettings.optimizing=true;
	optimizeSettings.pause=0;
	SDL_CreateThread(OptimizeThreadMain, this);
}


void SimulationManager::InteractiveTick()
{
	assert(!isOptimizing);

	int simticks = config.simLength * optimizeSettings.hz;
	if (bestRun && tick == simticks)
		interactiveSettings.pause = 1;

	test->Step(&interactiveSettings);

	if (tick<simticks) {
		CheckHash();
		tick ++;
	}
}

void SimulationManager::ChangeTest( TestFactory *f )
{
	assert(!isOptimizing);
	testFactory = f;
	SetTest(testFactory->createFcn());
}

std::vector<ParameterRange> SimulationManager::GetRanges()
{
	std::vector<ParamInfo> inputs = test->GetParamInfo();

	std::vector<ParameterRange> ranges (inputs.size());
	for(int i=0;i<ranges.size();i++) {
		ranges[i].min=inputs[i].min;
		ranges[i].max=inputs[i].max;
	}

	return ranges;
}


std::string SimulationManager::GetInfoString()
{
	if (isOptimizing)
		return "Optimizing....";
	else
		return SPrintf("%s. Time:%.2f. Score: %.2f. Tick: %d. Hash: %X", 
		testFactory->name, test->GetTime(), test->GetScore(), tick, test->CalcHash());
}

void SimulationManager::Reset()
{
	SetTest(testFactory->createFcn());
	if (bestRun) {
		test->SetControlParams(&bestRun->controlState.front());

		tick = -1;
		CheckHash();
		tick = 0;
	}
}

void SimulationManager::StopOptimization()
{
	stopOptimization = true;
}

void SimulationManager::SetTest( Test* t )
{
	SafeDelete(test);
	test=t;
	test->SetupListeners();
	tick = 0;
	interactiveSettings.pause=0;
	hashFailed=false;
}

void SimulationManager::CheckHash()
{
	if (hashFailed)
		return;

	if (bestRun) {
		uint hash = test->CalcHash();
		uint wantedHash = bestRun->hashHistory[tick+1];

		if (hash != wantedHash) {
			d_trace("Tick %d: Hash fail! %X should be %X\n", tick, hash, wantedHash );
			hashFailed=true;
		}
	}
}

