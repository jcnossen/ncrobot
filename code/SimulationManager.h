
#pragma  once

#include "Optimizer.h"
#include "Test.h"

class SimulationConfig
{
public:
	SimulationConfig() {
		numSimThreads=4;
		simLength=10.0f;
		maxTicks=-1;
		maxTime=120;
		population = 20;
		patience = -1;
	}

	int numSimThreads;
	float simLength; // 5 second sim
	int maxTicks;
	int maxTime;  // seconds
	int patience;
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

	void DrawInfo(int sy);

	// Start optimization (non-blocking)
	void StartOptimization(SimulationConfig cfg);
	// Optimize and return (blocking)
	void Optimize(SimulationConfig cfg);
	void ChangeTest(TestEntry *f);
	void StopOptimization();
	bool IsOptimizing() { return isOptimizing; }
	Test* GetTest() { return test; }
	void Reset();
	void SetTest(Test* t, TestEntry*f);

	std::string GetInfoString();
	std::vector<ParameterRange> GetRanges();

	TestSettings interactiveSettings;

	void WriteMatlabData(std::string file, std::string header);

protected:
	Test* test;
	TestSettings optimizeSettings;
	TestEntry* testEntry;
	Optimizer* optimizer;
	bool isOptimizing, stopOptimization;
	SimulationConfig config;
	uint optimizerStartTime; // SDL_GetTicks() when StartOptimization was called
	int tick;
	bool hashFailed; // to prevent spamming console

	struct RunInfo {
		RunInfo() { score=0.0f; iteration = 0;}
		std::vector<float> controlState;
		std::vector<uint> hashHistory;
		float score;
		int iteration;
	};

	std::list<RunInfo> runHistory;
	RunInfo* bestRun;

	static int OptimizeThreadMain(void *p);
	void Optimize();
};

