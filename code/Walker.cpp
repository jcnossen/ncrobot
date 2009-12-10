
#include "StdIncl.h"

#include "Render.h"
#include "Test.h"

//#include "../Box2D/Examples/TestBed/Tests/TheoJansen.h"

class Walker : public Test
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
		inp.min = -40.0f;
		inp.max = 40.0f;
		inputs.push_back(inp);
		inputs.push_back(inp);
		inp.min = -10.0f;
		inp.max = 10.0f;
		inputs.push_back(inp);
		inputs.push_back(inp);
	}

	void UpdateMotors()
	{
		for(int i=0;i<motors.size();i++) {
			int f = motors[i].paramIndex;
			//float s = params[f] + params[f+1] * cosf(params[f+2] * m_time);
			float s = params[f] + params[f+1] * cosf(params[f+2] * 0.01f * m_time - params[f+3] * 0.01f);
			motors[i].joint->SetMotorSpeed(s);
		}
	}

	float GetScore()
	{
		return chassis->GetWorldCenter().x;
	//	return chassis->GetLinearVelocity().x;
	}

	void CreateLeg(b2Vec2 start)
	{
		float mmt=2000.0f;
		float kneeY = m_offset.y*0.5f;
		// upper leg, distance joint to chassis

		b2BodyDef upperDef;
		upperDef.position = b2Vec2(start.x, start.y*0.5f);
		
		Motor m;
		m.body = m_world->CreateBody(&upperDef);
		b2CircleDef cd;
		cd.density=1.0f;
		cd.radius=1.0f;
		cd.filter.groupIndex=1;
		m.body->CreateShape(&cd);
		m.body->SetMassFromShapes();

		b2RevoluteJointDef jd;
		jd.Initialize(chassis, m.body, start);
		jd.enableMotor=true;
		jd.maxMotorTorque=mmt;
		jd.enableLimit=false;
		jd.lowerAngle = -3.14*0.5;
		jd.upperAngle = 3.14*0.5;
		m.joint =(b2RevoluteJoint*)m_world->CreateJoint(&jd);
		AddMotor(m);

		// lower leg
		b2PolygonDef foot;
		foot.SetAsBox(0.8f, 0.4f);
		foot.density=1.0f;
		foot.filter.groupIndex =1;
		Motor f;
		b2BodyDef fd;
		fd.position=b2Vec2(start.x, 0.0f);
		f.body = m_world->CreateBody(&fd);
		f.body->CreateShape(&foot);
		f.body->SetMassFromShapes();
		jd.Initialize(m.body, f.body, m.body->GetWorldCenter());
		jd.enableMotor=true;
		jd.maxMotorTorque=mmt;
		jd.enableLimit=false;
		f.joint=(b2RevoluteJoint*)m_world->CreateJoint(&jd);

		AddMotor(f);
	}

	Walker(int legs)
	{
		m_offset.Set(0.0f, 10.0f);

		float ld = 6.0f;
		float w = ld * legs;

		{
			b2PolygonDef sd;
			float e=300.0f;
			sd.SetAsBox(e, 10.0f);

			b2BodyDef bd;
			bd.position.Set(0.0f, -10.0f);
			b2Body* ground = m_world->CreateBody(&bd);
			ground->CreateShape(&sd);

			sd.SetAsBox(0.5f, 5.0f, b2Vec2(-e, 15.0f), 0.0f);
			ground->CreateShape(&sd);

			sd.SetAsBox(0.5f, 5.0f, b2Vec2(e, 15.0f), 0.0f);
			ground->CreateShape(&sd);
		}

		{
			b2PolygonDef sd;
			sd.density = 0.2f;
			sd.SetAsBox(w*0.5f, 1.0f);
			sd.filter.groupIndex = 1;
			b2BodyDef bd;
			bd.position = m_offset;
			chassis = m_world->CreateBody(&bd);
			chassis->CreateShape(&sd);
			chassis->SetMassFromShapes();
		}

		for(int x=0;x<legs;x++) {
			CreateLeg(b2Vec2(-w/2+ld*(x+0.5f), m_offset.y));
		}

		// make a wheel

		
	/*	b2BodyDef upperDef;
		upperDef.position = b2Vec2(m_offset.x+2, m_offset.y);
		Motor m;
		m.body = m_world->CreateBody(&upperDef);
		b2CircleDef cd;
		cd.density=1.0f;
		cd.radius=2.0f;
		cd.filter.groupIndex=-1;
		m.body->CreateShape(&cd);
		m.body->SetMassFromShapes();

		b2RevoluteJointDef jd;
		jd.Initialize(chassis, m.body, upperDef.position);
		jd.enableMotor=true;
		jd.maxMotorTorque=2000;
		m.joint =(b2RevoluteJoint*)m_world->CreateJoint(&jd);
		AddMotor(m);

		CreateLeg(m_offset);*/
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


};


Test* CreatePSOWalker2()
{
	return new Walker(2);
}

Test* CreatePSOWalker4()
{
	return new Walker(4);
}

Test* CreatePSOWalker10()
{
	return new Walker(10);
}

TestEntry g_testEntries[] =
{
	{"2-leg walker", CreatePSOWalker2},
	{"4-leg walker", CreatePSOWalker4},
	{"10-leg walker", CreatePSOWalker10},
	{0, 0	}
};

