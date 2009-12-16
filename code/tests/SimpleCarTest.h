
class SimpleCarTest : public Test
{
public:
	b2Vec2 m_offset;
	b2Body* chassis;

	std::vector<b2RevoluteJoint*> wheels;

	float GetScore()
	{
		b2Vec2 p = chassis->GetWorldCenter();
		return p.x;
		//	return chassis->GetLinearVelocity().x;
	}

	SimpleCarTest()
	{
		m_offset.Set(0.0f, 10.0f);

		float ld = 10.0f;

		b2PolygonDef sd;
		sd.density = 0.2f;
		sd.SetAsBox(7.0f, 1.0f);
		sd.filter.groupIndex = 0;
		b2BodyDef bd;
		bd.position = m_offset;
		chassis = world->CreateBody(&bd);
		chassis->CreateShape(&sd);
		chassis->SetMassFromShapes();

		AddWheel(b2Vec2(m_offset.x+3, m_offset.y));
		AddWheel(b2Vec2(m_offset.x-3, m_offset.y));
	}

	void AddWheel(b2Vec2 pos) {
		b2BodyDef upperDef;
		upperDef.position = pos;
		b2Body* body = world->CreateBody(&upperDef);
		b2CircleDef cd;
		cd.density=1.0f;
		cd.radius=2.0f;
		cd.filter.groupIndex=-1;
		body->CreateShape(&cd);
		body->SetMassFromShapes();

		b2RevoluteJointDef jd;
		jd.Initialize(chassis, body, upperDef.position);
		jd.enableMotor=true;
		jd.maxMotorTorque=2000;

		wheels.push_back((b2RevoluteJoint*)world->CreateJoint(&jd));
		inputs.push_back(ParamInfo(-50,50));
	}

	void Step(TestSettings* settings) {

		if (!params.empty()) {
			for(int i=0;i<wheels.size();i++)
				wheels[i]->SetMotorSpeed(params[i]);
		}

		Test::Step(settings);
	}


	static Test* Create() {
		return new SimpleCarTest();
	}
};


