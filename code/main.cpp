/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "StdIncl.h"

#include "glui/GL/glui.h"

#include "Render.h"
#include "Test.h"

#include "swarm/Swarm.h"
#include "GAOptimizer.h"

#include "ThreadManager.h"




namespace
{
	int32 testIndex = 0;
	int32 testSelection = 0;
	int32 selectedOptimizer = 0;
	TestEntry* entry;
	Test* test = 0;
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
	int swarmTicks=20;
	int numSimThreads=4;
	float simLength = 5.0f; // 5 second sim
	bool playback=false;
	std::vector<float> curCtlParams;

	SwarmConfig swarmConfig;
}

typedef Optimizer* OptimizerCreateFn();

struct OptimizerEntry {
	OptimizerCreateFn *createFn;
	const char*name;
};

Optimizer* CreateSwarm() { return new Swarm(swarmConfig); }
Optimizer* CreateGA() { return new GAOptimizer(); }
//Optimizer* CreateSwarm() { return new Swarm(swarmConfig); }

OptimizerEntry optimizers[] = {
	{ CreateSwarm, "Particle Swarm" },
	{ CreateGA, "Genetic Algorithm"	},
//	{ CreateSA, "Simulated Annealing" },
	{ 0, 0}
};


void Resize(int32 w, int32 h)
{
	width = w;
	height = h;

	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);

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

void SetupTest(std::vector<float>* ctlParams=0)
{
	delete test;
	entry = g_testEntries + testIndex;
	test = entry->createFcn();
	test->SetupListeners();

	if (ctlParams) {
		curCtlParams = *ctlParams;
		test->SetControlParams(&curCtlParams[0]);
		playback=true;
	}
}

void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	test->SetTextLine(30);
	settings.hz = settingsHz;

	if (playback && test->GetTime() >= simLength) 
		SetupTest(&curCtlParams);

	test->Step(&settings);

	DrawString(5, 15, entry->name);

	glutSwapBuffers();

	if (testSelection != testIndex)
	{
		testIndex = testSelection;
		SetupTest();
		viewZoom = 1.0f;
		viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
	}
}

void Keyboard(unsigned char key, int x, int y)
{
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

		// Press 'r' to reset.
	case 'r':
		delete test;
		b2Assert(b2_byteCount == 0);
		test = entry->createFcn();
		break;

	default:
		if (test)
		{
			test->Keyboard(key);
		}
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
		{
			rMouseDown = false;
		}
	}
}

void MouseMotion(int32 x, int32 y)
{
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

void Pause(int)
{
	settings.pause = !settings.pause;
}

void SingleStep(int)
{
	settings.pause = 1;
	settings.singleStep = 1;
}

void Reset(int)
{
	SetupTest();
}

struct TestWorkItemParam
{
	Test* test;
	float score;
	int numTicks;
	int index;
};

void RunTest_WorkItem(void* d) {
	TestWorkItemParam* p = (TestWorkItemParam*)d;
//	printf("j=%d; %p\n",p->index,p);
	
	// run simulation, and feed scores back into swarm
	p->test->SetupForPSO();
	for(int u=0;u<p->numTicks;u++)
		p->test->Step(&settings);
	
	p->score = p->test->GetScore();
}

void RunSwarm(int)
{
	float bestScore=-100000.0f;
	std::vector<float> best;
	std::vector<ParamInfo> inputs = test->GetParamInfo();
	std::vector<ParameterRange> ranges;

	int simticks = settings.hz * simLength;

	settings.psoRun = true;

	ranges.resize(inputs.size());
	for(int i=0;i<ranges.size();i++) {
		ranges[i].min=inputs[i].min;
		ranges[i].max=inputs[i].max;
	}

	Optimizer* optimizer = optimizers[selectedOptimizer].createFn();

	swarmConfig.paramRanges = ranges;
	Swarm sw(swarmConfig);

	bool useThreads=true;

	TestEntry& entry = g_testEntries[testIndex];

	for (int i=0;i<swarmTicks;i++) {
		d_trace("Swarm update tick: %d\n", i);

		sw.update();
		ThreadManager threadManager;
		std::vector<TestWorkItemParam> workItemParams(swarmConfig.popSize);

		// post items for the simulation threads
		for(int j=0;j<swarmConfig.popSize;j++) {
			Particle& sp = sw.swarm[j];
			Test* t = entry.createFcn();
			t->SetControlParams(&sp.position[0]);
			workItemParams[j].test = t;
			workItemParams[j].numTicks = simticks;
			workItemParams[j].index=j;

			if (useThreads) {
				threadManager.AddWorkItem(RunTest_WorkItem, &workItemParams[j]);
			} else
				RunTest_WorkItem(&workItemParams[j]);
		}

		if (useThreads) {
			threadManager.Start(numSimThreads);
			threadManager.WaitForAllDone();
		}

		// Update best score
		for(int j=0;j<swarmConfig.popSize;j++) {
			Particle& sp = sw.swarm[j];
			delete workItemParams[j].test;
			workItemParams[j].test=0;
			sp.fitness = workItemParams[j].score;
			if (sp.fitness > bestScore){ 
				bestScore=sp.fitness;
				best=sp.position;
			}
		}

		d_trace("; BestLen: %f\n", bestScore);
	}

	SetupTest(&best);

	for(int a=0;a<best.size();a++) {
		d_trace("Param[%d]: %f\n", a,best[a]);
	}

	settings.psoRun = false;
}



int main(int argc, char** argv)
{
	SetupTest();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
	mainWindow = glutCreateWindow(title);
	//glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(SimulationLoop);
	GLUI_Master.set_glutReshapeFunc(Resize);  
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutMouseFunc(Mouse);
#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);

	glui = GLUI_Master.create_glui_subwindow( mainWindow, 
		GLUI_SUBWINDOW_RIGHT );

	glui->add_statictext("Tests");
	GLUI_Listbox* testList = glui->add_listbox("", &testSelection);

	for(int i=0;g_testEntries[i].createFcn;i++)
		testList->add_item(i, g_testEntries[i].name);

	glui->add_statictext("Optimizer");
	GLUI_Listbox* optimList = glui->add_listbox("", &selectedOptimizer);
	for(int i=0;optimizers[i].createFn;i++)
		optimList->add_item(i, optimizers[i].name);

	glui->add_separator();

	glui->add_statictext("Swarm graph type");
	GLUI_Listbox* graphTypeList = glui->add_listbox("", &swarmConfig.graphType);
	for(int i=0;i<Swarm::numGraphTypes();i++)
		graphTypeList->add_item(i, Swarm::graphTypeNames[i]);
	glui->add_separator();


/*	GLUI_Spinner* iterationSpinner =
		glui->add_spinner("Iterations", GLUI_SPINNER_INT, &settings.iterationCount);
	iterationSpinner->set_int_limits(1, 100);

	GLUI_Spinner* hertzSpinner =
		glui->add_spinner("Hertz", GLUI_SPINNER_FLOAT, &settingsHz);

	hertzSpinner->set_float_limits(5.0f, 200.0f);
*/

// 	glui->add_checkbox("Position Correction", &settings.enablePositionCorrection);
// 	glui->add_checkbox("Warm Starting", &settings.enableWarmStarting);
// 	glui->add_checkbox("Time of Impact", &settings.enableTOI);

	glui->add_spinner("Swarm ticks", GLUI_SPINNER_INT, &swarmTicks);
	glui->add_spinner("Swarm size", GLUI_SPINNER_INT, &swarmConfig.popSize);
	glui->add_checkbox("Motor control", &settings.motorCtl);
	glui->add_spinner("Simulation Threads", GLUI_SPINNER_INT, &numSimThreads);
	glui->add_spinner("Sim length for optimizer", GLUI_SPINNER_FLOAT, &simLength);

	glui->add_separator();

	GLUI_Panel* drawPanel =	glui->add_panel("Draw");
	glui->add_checkbox_to_panel(drawPanel, "Shapes", &settings.drawShapes);
	glui->add_checkbox_to_panel(drawPanel, "Joints", &settings.drawJoints);
//	glui->add_checkbox_to_panel(drawPanel, "Core Shapes", &settings.drawCoreShapes);
//	glui->add_checkbox_to_panel(drawPanel, "AABBs", &settings.drawAABBs);
//	glui->add_checkbox_to_panel(drawPanel, "OBBs", &settings.drawOBBs);
//	glui->add_checkbox_to_panel(drawPanel, "Pairs", &settings.drawPairs);
// 	glui->add_checkbox_to_panel(drawPanel, "Contact Points", &settings.drawContactPoints);
// 	glui->add_checkbox_to_panel(drawPanel, "Contact Normals", &settings.drawContactNormals);
// 	glui->add_checkbox_to_panel(drawPanel, "Contact Forces", &settings.drawContactForces);
// 	glui->add_checkbox_to_panel(drawPanel, "Friction Forces", &settings.drawFrictionForces);
// 	glui->add_checkbox_to_panel(drawPanel, "Center of Masses", &settings.drawCOMs);
// 	glui->add_checkbox_to_panel(drawPanel, "Statistics", &settings.drawStats);

	//glui->add_button("Deactivate panel", 0, DeactivatePanel);


	glui->add_button("Swarm optimization",0, RunSwarm);
	glui->add_button("Reset", 0, Reset);
	glui->add_button("Single Step", 0, SingleStep);

	glui->add_button("Quit", 0,(GLUI_Update_CB)exit);
	glui->set_main_gfx_window( mainWindow );

	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);

	glutMainLoop();

	return 0;
}
