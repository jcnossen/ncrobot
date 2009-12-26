#pragma once

#include "glui/GL/glui.h"
#include <SDL_thread.h>
#include <list>

class GraphControl;

class Graph {
public:
	Graph();
	~Graph();

	void setData(int dimOfs, float* ys, int ndims, float fitness);
	void startTick();
	void draw();
	void setup(int dim);

private:
	int glutWindow;
	GLUI* glui;
	GLUI_Listbox* paramList;
	GraphControl* graphControl;

	struct ParamState {
		float val, fitness;
	};

	typedef std::list<std::vector<ParamState> > DataList;
	DataList data;
	int dim;
	int tick;
	float minFitness;

	// to make setValue thread-safe
	SDL_mutex* mutex;

	static void uiClose(int);
};
