
#include "StdIncl.h"

#include "Render.h"
#include "Test.h"

//#include "../Box2D/Examples/TestBed/Tests/TheoJansen.h"

class PSOWalker : public Test
{
public:
	b2Vec2 m_offset;
	b2Body* chassis;

	struct Motor {
		b2Body* body;
		b2RevoluteJoint* joint;
	};

	std::vector<PSOInput> inputs;
	std::vector<Motor> motors;

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
		m.body->CreateShape(&cd);
		m.body->SetMassFromShapes();

		b2RevoluteJointDef jd;
		jd.Initialize(chassis, m.body, m.body->GetWorldCenter());
		m.joint =(b2RevoluteJoint*)m_world->CreateJoint(&jd);
		motors.push_back(m);

		// lower leg
		b2PolygonDef foot;
		foot.SetAsBox(0.8f, 0.4f);
		foot.density=1.0f;
		Motor f;
		b2BodyDef fd;
		fd.position=b2Vec2(a1, 0.0f);
		f.body = m_world->CreateBody(&fd);
		f.body->CreateShape(&foot);
		f.body->SetMassFromShapes();
		jd.Initialize(m.body, f.body, m.body->GetWorldCenter());
		f.joint=(b2RevoluteJoint*)m_world->CreateJoint(&jd);
	}

	PSOWalker()
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

		CreateLeg(0.5f, -0.5f);
		CreateLeg(-0.5f, 0.5f);

	}

	void Step(TestSettings* settings)
	{
		if (!settings->psoRun) {
			DrawString(5, m_textLine, "Keys: left = a, brake = s, right = d, toggle motor = m");
			m_textLine += 15;
		}

		Test::Step(settings);
	}

	void Keyboard(unsigned char key)
	{
	}

	std::vector<PSOInput> getPSOInputs()
	{
		return inputs;
	}


};


Test* CreatePSOWalker()
{
	return new PSOWalker();
}

