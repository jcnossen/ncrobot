/*
ncrobot
Natural Computing Robot
This app is based on the TestBed example application from Box2D.

Copyright 2009 Jelmer Cnossen, particle swarm optimization code by Johan de Ruiter
License (ZLIB):

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/

#include "StdIncl.h"

#include <SDL.h>
#include <time.h>

#include "glui/GL/glui.h"

#include "Render.h"
#include "Test.h"
#include "Graph.h"
#include "swarm/Swarm.h"
#include "GAOptimizer.h"
#include "CfgParser.h"

#include "SimulationManager.h"

namespace
{
	int32 testIndex = 0;
	int32 testSelection = 0;
	int32 selectedOptimizer = 0;
	TestSettings settings;
	int32 width = 640;
	int32 height = 480;
	int32 framePeriod = 16;
	int32 mainWindow;
	float settingsHz = 60.0;
	GLUI *glui;
	float32 viewZoom = 1.0f;
	b2Vec2 viewCenter(0.0f, 20.0f);
	int tx, ty, tw, th;
	bool rMouseDown;
	b2Vec2 lastp;
	int32 showGraph=0;
	SwarmConfig swarmConfig;
	GLUI_Button* optimizeButton;
	GLUI_EditText* exportEditText;

	SimulationConfig simConfig;
	SimulationManager *simManager;

	std::string loadParamSetFile; 
	Graph* graph;
}

static std::vector<TestEntry> testEntries;



typedef Optimizer* OptimizerCreateFn();

struct OptimizerEntry {
	OptimizerCreateFn *createFn;
	const char *name, *id;
};

Optimizer* CreateSwarm() { 
	swarmConfig.popSize= simConfig.population;
	return new Swarm(swarmConfig, simManager->GetRanges());
}

Optimizer* CreateGA() { 
	GAConfig cfg;
	cfg.crossoverMutationRate = 0.1f;
	cfg.pointMutationRate = 0.1f;
	cfg.size = simConfig.population;
	return new GAOptimizer(cfg, simManager->GetRanges()); 
}
//Optimizer* CreateSwarm() { return new Swarm(swarmConfig); }

OptimizerEntry optimizers[] = {
	{ CreateSwarm, "Particle Swarm", "pso" },
	{ CreateGA, "Genetic Algorithm", "ga"	},
//	{ CreateSA, "Simulated Annealing" },
	{ 0, 0}
};


void Resize(int32 w, int32 h)
{
	width = w;
	height = h;

 	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
 	glViewport(tx, ty, tw, th);

//	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float32 ratio = float32(tw) / float32(th);

	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = viewCenter - extents;
	b2Vec2 upper = viewCenter + extents;

	// L/R/B/T
	gluOrtho2D(lower.x, upper.x, lower.y, upper.y);
}

b2Vec2 ConvertScreenToWorld(int32 x, int32 y)
{
	float32 u = x / float32(tw);
	float32 v = (th - y) / float32(th);

	float32 ratio = float32(tw) / float32(th);
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = viewCenter - extents;
	b2Vec2 upper = viewCenter + extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
}

// This is used to control the frame rate (60Hz).
void Timer(int)
{
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 0);
}


void SimulationLoop()
{
	if (simManager->IsOptimizing())
		optimizeButton->set_name("Stop optimization");
	else
		optimizeButton->set_name("Run optimization");

	glClearColor(0.4f,0.4f,0.4f,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (testSelection != testIndex)
	{
		testIndex = testSelection;
		simManager->ChangeTest(&testEntries[testIndex]);

		viewZoom = 1.0f;
		viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
	}

	if (!simManager->IsOptimizing()) {
		simManager->interactiveSettings.hz = settingsHz;
		simManager->InteractiveTick();
	}

	simManager->DrawInfo(15);

	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
	Test* test = simManager->GetTest();
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
	case 27:
		exit(0);
		break;

		// Press 'z' to zoom out.
	case 'z':
		viewZoom = b2Min(1.1f * viewZoom, 20.0f);
		Resize(width, height);
		break;

		// Press 'x' to zoom in.
	case 'x':
		viewZoom = b2Max(0.9f * viewZoom, 0.02f);
		Resize(width, height);
		break;

	case 'r':
		simManager->Reset();
		break;

	default:
		if (test)
			test->Keyboard(key);
	}
}

void KeyboardSpecial(int key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
		// Press left to pan left.
	case GLUT_KEY_LEFT:
		viewCenter.x -= 0.5f;
		Resize(width, height);
		break;

		// Press right to pan right.
	case GLUT_KEY_RIGHT:
		viewCenter.x += 0.5f;
		Resize(width, height);
		break;

		// Press down to pan down.
	case GLUT_KEY_DOWN:
		viewCenter.y -= 0.5f;
		Resize(width, height);
		break;

		// Press up to pan up.
	case GLUT_KEY_UP:
		viewCenter.y += 0.5f;
		Resize(width, height);
		break;

		// Press home to reset the view.
	case GLUT_KEY_HOME:
		viewZoom = 1.0f;
		viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
		break;
	}
}

void Mouse(int32 button, int32 state, int32 x, int32 y)
{
	Test* test = simManager->GetTest();

	// Use the mouse to move things around.
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			b2Vec2 p = ConvertScreenToWorld(x, y);
			test->MouseDown(p);
		}
		
		if (state == GLUT_UP)
		{
			test->MouseUp();
		}
	} else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{	
			lastp = ConvertScreenToWorld(x, y);
			rMouseDown = true;
		}

		if (state == GLUT_UP)
			rMouseDown = false;
	}
}

void MouseMotion(int32 x, int32 y)
{
	Test* test = simManager->GetTest();
	b2Vec2 p = ConvertScreenToWorld(x, y);
	test->MouseMove(p);
	
	if (rMouseDown){
		b2Vec2 diff = p - lastp;
		viewCenter.x -= diff.x;
		viewCenter.y -= diff.y;
		Resize(width, height);
		lastp = ConvertScreenToWorld(x, y);
	}
}

void MouseWheel(int wheel, int direction, int x, int y)
{	B2_NOT_USED(wheel);
	B2_NOT_USED(x);
	B2_NOT_USED(y);
	if (direction > 0) {
		viewZoom /= 1.1f;
	} else {
		viewZoom *= 1.1f;
	}
	Resize(width, height);
}

void Reset(int)
{
	simManager->Reset();
}

void RunOptimization(int) 
{
	if(simManager->IsOptimizing()) {
		simManager->StopOptimization();
	}
	else {
		Optimizer* optimizer = optimizers[selectedOptimizer].createFn();
		simManager->SetOptimizer(optimizer);
		simManager->StartOptimization(simConfig);
	}
}


void ShowGraph(int)
{
	if (!graph) 
		graph = new Graph();
}

void ExportAnim(int)
{
	const char* filename = exportEditText->get_text();
	CfgWriter w(filename);
	if (w.HasFailed()) {
		d_trace("Unable to open %s for writing\n",filename);
		return;
	}

	CfgList* cfg = new CfgList();
	cfg->Add("test", testIndex);
	CfgList* pl = new CfgList();
	cfg->Add("params", pl);
	
	float* params = simManager->GetTest()->GetControlParams();
	if(!params)  {
		d_trace("Export failed, no control params\n");
	} else {
		int s = simManager->GetRanges().size();
		for(int a=0;a<s;a++)
			pl->Add(params[a]);
		cfg->Write(w);
	}

	delete cfg;
}


void CollectTestEntries() {
	std::vector<TestFactoryBase*> fl = TestFactoryBase::getFactoryList();

	for(int i=0;i<fl.size();i++) {
		for (int j=0;j<fl[i]->getNumTests();j++)  {
			TestEntry e;
			e.factory = fl[i];
			e.index = j;
			e.name = fl[i]->getTestName(j);
			testEntries.push_back(e);
		}
	}
}


void LoadDemoConfig(){
	CfgList* l = CfgValue::LoadFile(loadParamSetFile);

	testSelection = testIndex = l->GetInt("test", 0);
	TestEntry* testEntry = &testEntries[testIndex];
	CfgList* paramList = l->GetList("params");
	
	std::vector<float> params;

	for(CfgList::iterator li=paramList->begin();li!=paramList->end();++li) {
		CfgValue* v = li->value;
		if (v->getType() != CfgValue::Numeric)
			throw ContentException("Parse error in demo");
		params.push_back(((CfgNumeric*)v )->value);
	}

	Test* test = testEntry->create();
	if (params.size() != test->GetParamInfo().size()) 
		throw ContentException(SPrintf("Invalid number of test parameters. Should be %d instead of %d", test->GetParamInfo().size(), params.size()));
	
	test->SetControlParams(&params.front());
	simManager->SetTest(test, testEntry);
}

void PrintHelp()
{
	printf("ncrobot <options>\n"
		"-cl\tCommand line only, no UI\n"
		"-ps <value>\tSet population size\n"
		"-len <seconds>\tSet length of simulation\n"
		"-maxticks <ticks>\tSet the max number of ticks, defaults to -1 for no max\n"
		"-maxtime <seconds>\tSet time limit for optimization run\n"
		"-threads <int>\tSet # of CPU threads used, default 4\n"
		"-topology <int>\tSet topology type [0-4]\n"
		"-test <int>\tSet test index\n"
		"-log <log file>\tSet debug output file\n"
		"-load <demo config file>\tPlayback demo (requires UI)\n"
		"-mlreport <matlab script output>\tSet target for matlab code output\n"
		"-srand <int>\tSet random seed\n"
		);
}

bool ParseCmdLine(int argc, char **argv) {
	bool cmdLineRun = false;
	std::string matlabReportFile;

	int maxTicks=-1,maxTime=-1,patience=-1;

	for(int a=1;a<argc;a++) {
		if (!STRCASECMP(argv[a], "-test") && a<argc-1) 
			testSelection = testIndex = atoi(argv[++a]);
		else if (!STRCASECMP(argv[a], "-cl"))
			cmdLineRun=true;
		else if (!STRCASECMP(argv[a], "-ps") && a<argc-1) 
			simConfig.population = atoi(argv[++a]);
		else if (!STRCASECMP(argv[a], "-len") && a<argc-1)
			simConfig.simLength = atoi(argv[++a]);
		else if (!STRCASECMP(argv[a], "-maxticks") && a<argc-1)
			maxTicks = atoi(argv[++a]);
		else if (!STRCASECMP(argv[a], "-maxtime") && a<argc-1)
			maxTime = atoi(argv[++a]);
		else if (!STRCASECMP(argv[a], "-patience") && a<argc-1)
			patience = atoi(argv[++a]);
		else if (!STRCASECMP(argv[a], "-topology") && a<argc-1)
			swarmConfig.graphType = atoi(argv[++a]);
		else if (!STRCASECMP(argv[a], "-threads") && a<argc-1)
			simConfig.numSimThreads = atoi(argv[++a]);
		else if (!STRCASECMP(argv[a], "-log") && a<argc-1)
			d_setlogfile(argv[++a]);
		else if (!STRCASECMP(argv[a], "-mlreport") && a<argc-1) {
			matlabReportFile = argv[++a];
			d_trace("Writing to matlab script: %s\n", matlabReportFile.c_str());
		} 
		else if (!STRCASECMP(argv[a], "-load") && a<argc-1) {
			loadParamSetFile = argv[++a];
		}
		else if(!STRCASECMP(argv[a], "-srand") && a<argc-1) {
			srand(atoi(argv[++a]));
		}
		else if(!STRCASECMP(argv[a], "-h")) {
			PrintHelp();
			return true;
		}
	}

	if (cmdLineRun) {
		simConfig.maxTicks=maxTicks;
		simConfig.maxTime=maxTime;
		simConfig.patience=patience;

		uint start = SDL_GetTicks();

		simManager = new SimulationManager();
		TestEntry* te = &testEntries[testIndex];
		d_trace("Running test: %s\n", te->name);
		simManager->ChangeTest(&testEntries[testIndex]);
		simManager->SetOptimizer(CreateSwarm());
		simManager->Optimize(simConfig);

		float td = 0.001f* (SDL_GetTicks() - start);
		d_trace("Time taken: %.1f s\n", td);

		if (!matlabReportFile.empty()) {
			std::string header = SPrintf("%% Test: '%s', PS: %d, Sim len: %.1f, Optimize time taken: %.1f s\n",
				te->name, simConfig.population, simConfig.simLength, td);
			simManager->WriteMatlabData(matlabReportFile, header.c_str());
		}
		return true;
	}

	return false; // continue as normal
}

int main(int argc, char** argv)
{
	CollectTestEntries();

	srand(time(0));

	if (ParseCmdLine(argc, argv))
		return 0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	mainWindow = glutCreateWindow("Natural Computing Robot");
	
	glutDisplayFunc(SimulationLoop);
	GLUI_Master.set_glutReshapeFunc(Resize);  
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutMouseFunc(Mouse);

#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);

	glui = GLUI_Master.create_glui_subwindow( mainWindow, GLUI_SUBWINDOW_RIGHT );
//	glui = GLUI_Master.create_glui("Controls");

	glui->add_statictext("Tests");
	GLUI_Listbox* testList = glui->add_listbox("", &testSelection);
	for(int i=0;i<testEntries.size();i++)
		testList->add_item(i, testEntries[i].name);
	testList->set_int_val(testSelection);

	glui->add_statictext("Optimizer");
	GLUI_Listbox* optimList = glui->add_listbox("", &selectedOptimizer);
	for(int i=0;optimizers[i].createFn;i++)
		optimList->add_item(i, optimizers[i].name);

	glui->add_statictext("Swarm graph type");
	GLUI_Listbox* graphTypeList = glui->add_listbox("", &swarmConfig.graphType);
	for(int i=0;i<Swarm::numGraphTypes();i++)
		graphTypeList->add_item(i, Swarm::graphTypeNames[i]);
	graphTypeList->set_int_val(1);//swarmConfig.graphType);
	glui->add_separator();

	glui->add_checkbox("Playback best", &settings.motorCtl);

	GLUI_Panel* optimPanel = glui->add_panel("Optimization Settings");
	glui->add_spinner_to_panel(optimPanel, "Max Ticks", GLUI_SPINNER_INT, &simConfig.maxTicks);
	glui->add_spinner_to_panel(optimPanel, "Max Time (s)", GLUI_SPINNER_INT, &simConfig.maxTime);
	glui->add_spinner_to_panel(optimPanel, "Population Size", GLUI_SPINNER_INT, &simConfig.population);
	glui->add_spinner_to_panel(optimPanel, "CPU threads", GLUI_SPINNER_INT, &simConfig.numSimThreads);
	glui->add_spinner_to_panel(optimPanel, "Sim duration", GLUI_SPINNER_FLOAT, &simConfig.simLength);

	GLUI_Panel* drawPanel =	glui->add_panel("Draw");
	glui->add_checkbox_to_panel(drawPanel, "Shapes", &settings.drawShapes);
	glui->add_checkbox_to_panel(drawPanel, "Joints", &settings.drawJoints);

	optimizeButton = glui->add_button("Run optimization",0, RunOptimization);
	glui->add_button("Reset", 0, Reset);
//	glui->add_checkbox("Graph", &showGraph);

	glui->add_button("Export to file:", 0, ExportAnim);
	exportEditText = glui->add_edittext("");

	glui->add_button("Quit", 0,(GLUI_Update_CB)exit);
	glui->set_main_gfx_window( mainWindow );

	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);

	simManager = new SimulationManager();

	if (!loadParamSetFile.empty())
		LoadDemoConfig();
	else
		simManager->ChangeTest(&testEntries[testIndex]);

	glutMainLoop();

	SafeDelete(simManager);

	return 0;
}
