
#pragma  once

#include "Optimizer.h"
#include "Test.h"

class SimulationConfig
{
public:
	SimulationConfig() {
		numSimThreads=4;
		simLength=10.0f;
		maxTicks=100;
		maxTime=120;
		population = 20;
	}

	int numSimThreads;
	float simLength; // 5 second sim
	int maxTicks;
	int maxTime;  // seconds
	int population;
};

class SimulationManager 
{
public:
	SimulationManager();
	~SimulationManager();

	void InteractiveTick();

	void CheckHash();
	void SetOptimizer(Optimizer* optimizer);
	void StartOptimization(SimulationConfig cfg);
	void ChangeTest(TestFactory *f);
	void StopOptimization();
	bool IsOptimizing() { return isOptimizing; }
	Test* GetTest() { return test; }
	void Reset();
	void SetTest(Test* t);

	std::string GetInfoString();
	std::vector<ParameterRange> GetRanges();

	TestSettings interactiveSettings;

protected:
	Test* test;
	TestSettings optimizeSettings;
	TestFactory* testFactory;
	Optimizer* optimizer;
	bool isOptimizing, stopOptimization;
	SimulationConfig config;
	uint optimizerStartTime; // SDL_GetTicks() when StartOptimization was called
	int tick;
	bool hashFailed; // to prevent spamming console

	struct RunInfo {
		RunInfo() { score=0.0f; }
		std::vector<float> controlState;
		std::vector<uint> hashHistory;
		float score;
	};

	std::list<RunInfo> runHistory;
	RunInfo* bestRun;

	static int OptimizeThreadMain(void *p);
	void Optimize();
};

