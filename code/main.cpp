#include "StdIncl.h"
#include "Model.h"
#include "ModelState.h" 
#include "CfgParser.h"

#include <SDL.h>
#include <SDL_main.h>

#include "OpenGL.h"
#include "GUI.h"

using namespace ncrobot;

#ifdef WIN32
	#include <windows.h>
#endif

void SysMsg (std::string txt, bool error=true) 
{
#ifdef WIN32
	MessageBoxA(NULL, txt.c_str(), "Exception: ", error ? (MB_OK | MB_ICONERROR) : MB_OK);
	d_trace("Exception: %s\n", txt.c_str());
#else
	fprintf (error ? stderr : stdout, txt.c_str());
#endif
}

static CfgList* appConfig = 0;
static bool quitApp = false;
static int screenWidth, screenHeight;
static Vector2 mousePos;
static Font* font;
static Model* model;
static ModelState* modelState;
static SimulationConfig* simConfig;

static Uint32 lastTicks;
static Uint32 ticksRemainder=0;


static void HandleKeyDown (SDLKey key) {
	if (key == SDLK_ESCAPE)
		quitApp=true;

	if (key == SDLK_l)
		modelState->loadInitial(XForm());
}


static void HandleEvents() 
{
	SDL_Event event;

	while (SDL_PollEvent(&event) == 1) {
		if (event.type == SDL_KEYDOWN) {
			HandleKeyDown (event.key.keysym.sym);
		}

		if (event.type == SDL_QUIT)
			quitApp=true;

		if (event.type == SDL_MOUSEMOTION) {
			mousePos = Vector2(event.motion.x, screenHeight - event.motion.y - 1);
		}
	}
}

static void VideoInit()
{
	CfgList* screenCfg = appConfig->GetList("screen");

	if (!screenCfg)
		screenCfg = CfgList::EmptyList;

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, screenCfg->GetInt("depthBufferBits", 24));
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	int flags = SDL_OPENGL;

	screenWidth = screenCfg->GetInt("width", 700);
	screenHeight = screenCfg->GetInt("height", 500);

	if(!SDL_SetVideoMode(screenWidth, screenHeight, screenCfg->GetInt("bpp", 24), flags)) 
	{
		throw std::runtime_error("Failed to initialize display system");
	}

	SDL_ShowCursor(0);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);
	glMatrixMode(GL_MODELVIEW);
}

static void DrawMouseCursor(Vector2 pos) {
	glBegin(GL_TRIANGLES);
	glColor(Color::Blue);
	glVertex(pos);
	glVertex2f(pos.x + 8, pos.y - 10);
	glVertex2f(pos.x + 4, pos.y - 14);
	glEnd();
}

static void UpdateScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const int guiRectHeight=50;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(-30,30,-10,30);
		glMatrixMode(GL_MODELVIEW);

		modelState->draw();

		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);


	// ground
	glColorRGB(0x7f7f7f);
	glRecti(0, 0, screenWidth, guiRectHeight);

	font->render("Testing...", Vector2(30,0), Color::White);

	DrawMouseCursor(mousePos);
	SDL_GL_SwapBuffers();
}

static void UpdateTicks()
{
	Uint32 ticks = SDL_GetTicks();
	Uint32 diff = ticks - lastTicks;

	ticksRemainder += diff;
	const int simTickLength = 1000 / simConfig->ticksPerSec;
	while(ticksRemainder >= simTickLength) {

		if (modelState) {
			modelState->simulate(simConfig);
		}

		ticksRemainder -= simTickLength;
	}
	lastTicks = ticks;
}

int main(int argc, char* argv[])
{
    try {
		appConfig = CfgValue::LoadFile("../appconfig.txt");

		if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
			SysMsg(SPrintf("SDL fails: %s", SDL_GetError()));
			return -1;
		}

		VideoInit ();

		GUI_Init();
		font = Font::load ("../data/Zillah Modern Thin.ttf", 16);

		std::string modelFile = appConfig->GetLiteral("model", "data/robot.mdl");
		Model mdl ("../" + modelFile);
		model = &mdl;

		simConfig = new SimulationConfig(appConfig->GetList("sim_config"));

		modelState = new ModelState(model);
		modelState->loadInitial(XForm());

		d_trace("Model '%s' has %d parts and %d constraints\n", mdl.name.c_str(), mdl.parts.size(), mdl.constraints.size());

		lastTicks = SDL_GetTicks();

		while (!quitApp) {
			HandleEvents();

			if (SDL_GetAppState() & SDL_APPACTIVE) {
				UpdateScreen();

				UpdateTicks();
			}
		}

		delete modelState;
		delete font;
		GUI_Shutdown();
		
	} catch (ContentException e) {
		SDL_Quit();
		SysMsg(SPrintf("Content Exception: %s\n", e.what()));
	}
#ifndef _DEBUG
	catch (std::exception e) {
		SDL_Quit();
		SysMsg( SPrintf("Exception: %s", e.what()), true);
	}
#endif
	delete appConfig;

    return 0;
}
