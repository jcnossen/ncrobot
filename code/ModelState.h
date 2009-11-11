#pragma once


#include "XForm.h"

class CfgList;

namespace ncrobot {

	class Model;

	class SimulationConfig {
	public:
		SimulationConfig(CfgList* cfg);

		Vector2 gravity;
		int ticksPerSec;
	};

	class ModelState
	{
	public:
		ModelState(Model *mdl);
		~ModelState();

		void loadInitial(XForm xForm);
		void draw();
		void simulate(SimulationConfig *cfg);

	private:
		Model *model;

		// part state
		struct ps_t {
			ps_t() {
				angle=angularVel = 0.0f;
				momentAccum = 0.0f;
			}
			float angle;
			float angularVel;
			
			Vector2 position;
			Vector2 speed;

			Vector2 forceAccum;
			float momentAccum;
		};

		struct constraint_t {

		};

		int numParts;
		ps_t *state; // 1 ps_t per part
	};
}
