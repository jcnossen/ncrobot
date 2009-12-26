#include "StdIncl.h"

#include "Graph.h"
#include "glui/GL/glui.h"
#include "Color.h"

#include "OpenGL.h"


class GraphControl : public GLUI_Control {
public:
	GraphControl();

	Graph* owner;
	void draw(int x,int y);
};

void GraphControl::draw( int x,int y )
{
	owner->draw();
}

GraphControl::GraphControl() 
{
	w = 400;
	h = 300;
}

void menuFunc(int menu) {

}

void Graph::uiClose(int) {
	Graph* g = (Graph*)GLUI_Master.active_control->user_id;

	g->glui->add_button("Really?");
}

Graph::Graph()
{
	minFitness = 0.0f;
	glui = GLUI_Master.create_glui("Parameter Graph");

	GLUI_Button* close = glui->add_button("Close", 0, uiClose);
	close->user_id = (long)this;

//	glui->add_spinner("Min fitness", GLUI_SPINNER_FLOAT, &minFitness);

	GLUI_Panel *p = glui->add_panel("Graph");
	graphControl = new GraphControl();
	graphControl->owner=this;
	p->add_control(graphControl);

	mutex = SDL_CreateMutex();
}

Graph::~Graph()
{
	delete glui;
	SDL_DestroyMutex(mutex);
}

void Graph::setup(int dim) {
	this->dim = dim;
	data.clear();
}

void Graph::draw()
{
	SDL_LockMutex(mutex);

	if (data.empty()) {
		SDL_UnlockMutex(mutex);
		return;
	}

	// find ranges
	float minY=100, maxY=-100;
	for(DataList::iterator di = data.begin(); di != data.end(); ++di) {
		std::vector<ParamState> &d = *di;
		for(int i=0;i<d.size();i++) {
			if (d[i].val < minY)
				minY = d[i].val;
			if (d[i].val > maxY)
				maxY = d[i].val;
		}
	}

	int scrW = graphControl->w;
	int scrH = graphControl->h;

	float h = maxY-minY;
	if (h <= 0.01f) {
		h = 1.0f;
		minY -= 0.5f;
	}
	float xs=scrW/data.size();
	float ys = scrH/h;

	for (int i=0;i<dim;i++) {
		float last = 0.0f;
		int x =0;

		glBegin(GL_LINE_STRIP);
		glColor(Color::getGraphColor(i));
		for(DataList::iterator di = data.begin(); di != data.end(); ++di) {
			std::vector<ParamState> &d = *di;
			glVertex2f(x*xs, (d[i].val-minY) * ys);
			x++;
		}
		glEnd();
	}

	SDL_UnlockMutex(mutex);
}



void Graph::startTick()
{
	SDL_LockMutex(mutex);
	data.push_back(std::vector<ParamState>(dim));
	SDL_UnlockMutex(mutex);
}

void Graph::setData( int dimOfs, float* ys, int ndims, float fitness )
{
	SDL_LockMutex(mutex);
	for(int i=0;i<ndims;i++) {
		ParamState& s = data.back()[i+dimOfs];
		s.fitness = fitness;
		s.val = ys[i];
	}
	SDL_UnlockMutex(mutex);
}
