#include "StdIncl.h"
#include "ModelState.h"

#include "Model.h"
#include "Shape.h"
#include "CfgParser.h"

namespace ncrobot {

	SimulationConfig::SimulationConfig(CfgList* cfg)
	{
		ticksPerSec = cfg->GetInt("ticksPerSec", 60);
		gravity = cfg->GetVector2("gravity");
	}


	ModelState::ModelState(Model* mdl)
	{
		state = new ps_t[mdl->parts.size()];
		model = mdl;

		numParts=mdl->parts.size();
	}

	ModelState::~ModelState()
	{
		if (state) {
			delete[] state;
		}
	}

	void ModelState::loadInitial(XForm transform)
	{
		for(int i=0;i<numParts;i++) {
			state[i].speed = Vector2();
			state[i].forceAccum = Vector2();
			state[i].angle = 0.0f;
			state[i].position = model->parts[i]->center;
		}
	}

	void ModelState::simulate(SimulationConfig* simCfg) {
		const float dt = 1.0f / simCfg->ticksPerSec;

/*
Steps:
 - Find collisions, create collisions in 
 - Accumulate forces
 - Integrate
 */

		for (int a=0;a<numParts;a++) {
			ps_t& ps = state[a];

			ps.forceAccum += simCfg->gravity * model->parts[a]->mass.weight;

			// Damping?

			ps.forceAccum -= ps.speed * model->parts[a]->drag;
			ps.momentAccum -= ps.angularVel * model->parts[a]->angularDrag;
		}


		// Integrate (forward euler)
		for (int a=0;a<numParts;a++)
		{
			ps_t& ps = state[a];
			Part* part = model->parts[a];

			// Integrate position/angle
			ps.position += dt * ps.speed;
			ps.angle += dt * ps.angularVel;

			// Integrate speed/angular vel
			ps.angularVel += dt * ps.momentAccum / part->mass.J;
			ps.speed += dt * ps.forceAccum / part->mass.weight;

			// Reset accumulators
			ps.forceAccum = Vector2();
			ps.momentAccum = 0.0f;
		}
	}

	void ModelState::draw() {
		for(int a=0;a<numParts;a++) {

			Part *part = model->parts[a];

			if (part->shape) {
				part->shape->draw(XForm(state[a].position, state[a].angle));
			}
		}
	}
}
