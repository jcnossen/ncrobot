
class SimpleCarTest : public Test
{
public:
	b2Vec2 m_offset;
	b2Body* chassis;

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

		CreateBaseWorld();

		{
			b2PolygonDef sd;
			sd.density = 0.2f;
			sd.SetAsBox(7.0f, 1.0f);
			sd.filter.groupIndex = 0;
			b2BodyDef bd;
			bd.position = m_offset;
			chassis = m_world->CreateBody(&bd);
			chassis->CreateShape(&sd);
			chassis->SetMassFromShapes();
		}
		AddWheel(b2Vec2(m_offset.x+3, m_offset.y));
		AddWheel(b2Vec2(m_offset.x-3, m_offset.y));

	}

	void AddWheel(b2Vec2 pos) {
		b2BodyDef upperDef;
		upperDef.position = pos;
		b2Body* body = m_world->CreateBody(&upperDef);
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

		Motor m;
		m.joint =(b2RevoluteJoint*)m_world->CreateJoint(&jd);
		AddMotor(m);

	}

	void Keyboard(unsigned char key)
	{ }

	static Test* Create() {
		return new SimpleCarTest();
	}
};


