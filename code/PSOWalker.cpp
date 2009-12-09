
#include "StdIncl.h"

#include "Render.h"
#include "Test.h"

//#include "../Box2D/Examples/TestBed/Tests/TheoJansen.h"

class PSOWalker : public Test
{
public:
	b2Vec2 m_offset;
	b2Body* chassis;
	std::vector<float> params;

	struct Motor {
		b2Body* body;
		b2RevoluteJoint* joint;
		int paramIndex;
	};

	std::vector<ParamInfo> inputs;
	std::vector<Motor> motors;

	void AddMotor(Motor m)
	{
		m.paramIndex = inputs.size();
		motors.push_back(m);

		ParamInfo inp;
		inp.min = -100.0f;
		inp.max = 100.0f;
		inputs.push_back(inp);
		inputs.push_back(inp);
		inputs.push_back(inp);
	}

	void UpdateMotors()
	{
		for(int i=0;i<motors.size();i++) {
			int f = motors[i].paramIndex;
			float s = params[f] + params[f+1] * cosf(params[f+2] * m_time);

			motors[i].joint->SetMotorSpeed(s);
		}
	}


	void CreateLeg(float a0, float a1)
	{
		float kneeY = m_offset.y*0.5f;
		// upper leg, distance joint to chassis

		b2BodyDef upperDef;
		upperDef.position = b2Vec2(a0, kneeY);
		
		Motor m;
		m.body = m_world->CreateBody(&upperDef);
		b2CircleDef cd;
		cd.density=1.0f;
		cd.radius=1.0f;
		cd.filter.groupIndex=-1;
		m.body->CreateShape(&cd);
		m.body->SetMassFromShapes();

		b2RevoluteJointDef jd;
		jd.Initialize(chassis, m.body, chassis->GetWorldCenter());
		jd.enableMotor=true;
		jd.maxMotorTorque=100.0f;
		m.joint =(b2RevoluteJoint*)m_world->CreateJoint(&jd);
		AddMotor(m);

		// lower leg
		b2PolygonDef foot;
		foot.SetAsBox(0.8f, 0.4f);
		foot.density=1.0f;
		foot.filter.groupIndex =-1;
		Motor f;
		b2BodyDef fd;
		fd.position=b2Vec2(a1, 0.0f);
		f.body = m_world->CreateBody(&fd);
		f.body->CreateShape(&foot);
		f.body->SetMassFromShapes();
		jd.Initialize(m.body, f.body, m.body->GetWorldCenter());
		jd.enableMotor=true;
		jd.maxMotorTorque=100.0f;
		f.joint=(b2RevoluteJoint*)m_world->CreateJoint(&jd);

		AddMotor(f);
	}

	PSOWalker(int legs)
	{
		m_offset.Set(0.0f, 8.0f);

		{
			b2PolygonDef sd;
			sd.SetAsBox(100.0f, 10.0f);

			b2BodyDef bd;
			bd.position.Set(0.0f, -10.0f);
			b2Body* ground = m_world->CreateBody(&bd);
			ground->CreateShape(&sd);

			sd.SetAsBox(0.5f, 5.0f, b2Vec2(-100.0f, 15.0f), 0.0f);
			ground->CreateShape(&sd);

			sd.SetAsBox(0.5f, 5.0f, b2Vec2(100.0f, 15.0f), 0.0f);
			ground->CreateShape(&sd);
		}

		{
			b2PolygonDef sd;
			sd.density = 1.0f;
			sd.SetAsBox(2.5f, 1.0f);
			sd.filter.groupIndex = -1;
			b2BodyDef bd;
			bd.position = m_offset;
			chassis = m_world->CreateBody(&bd);
			chassis->CreateShape(&sd);
			chassis->SetMassFromShapes();
		}

		for(int x=0;x<legs/2;x++) {
			int v=x+1;
			CreateLeg(v,2*v);
			CreateLeg(-v,-2*v);
		}
	}

	void SetupForPSO()
	{
		m_world->SetDebugDraw(0);
		m_world->SetDestructionListener(0);
		m_world->SetBoundaryListener(0);
		m_world->SetContactFilter(0);
	}

	void Step(TestSettings* settings)
	{
		// Update motors based on time
		if (settings->psoRun)
			UpdateMotors();
		else {
			if (settings->motorCtl && params.size()>0)
				UpdateMotors();
			else {
				for(int i=0;i<motors.size();i++)
					motors[i].joint->EnableMotor(false);
			}
		}

		if (!settings->psoRun) {
			DrawString(5, m_textLine, "Keys: left = a, brake = s, right = d, toggle motor = m");
			m_textLine += 15;
		}

		Test::Step(settings);
	}

	void Keyboard(unsigned char key)
	{ }

	std::vector<ParamInfo> GetParamInfo()
	{
		return inputs;
	}
	
	void SetControlParams(float* v)
	{
		if(params.empty())
			params.resize(inputs.size());
		params.assign(v,v+params.size());
	}

	float GetScore()
	{
		return chassis->GetWorldCenter().Length();
	}

};


Test* CreatePSOWalker2()
{
	return new PSOWalker(2);
}

Test* CreatePSOWalker4()
{
	return new PSOWalker(4);
}


TestEntry g_testEntries[] =
{
	{"2-leg walker", CreatePSOWalker2},
	{"4-leg walker", CreatePSOWalker4},
	{0, 0	}
};

