#pragma once

//#include "../Box2D/Examples/TestBed/Tests/TheoJansen.h"


class Walker : public Test
{
	struct Motor {
		b2RevoluteJoint* joint;
		int paramIndex;
	};

	std::vector<Motor> motors;
public:

	b2Vec2 m_offset;
	b2Body* chassis;
	bool hasKnee, limits, collideKnees;

	void CreateLeg(b2Vec2 start)
	{
		float mmt=700.0f;
		float kneeY = m_offset.y*0.5f;
		// upper leg, distance joint to chassis

		b2BodyDef upperDef;
		upperDef.position = b2Vec2(start.x, start.y*0.5f);

		Motor m;
		b2Body* body;
		
		if (hasKnee) {
			body = m_world->CreateBody(&upperDef);
			b2CircleDef cd;
			cd.density=1.0f;
			cd.radius=1.0f;
			cd.filter.groupIndex=collideKnees ? 1:-1;
			body->CreateShape(&cd);
			body->SetMassFromShapes();

			b2RevoluteJointDef jd;
			jd.Initialize(chassis, body, start);
			jd.enableMotor=true;
			jd.maxMotorTorque=mmt;
			jd.enableLimit=true;
			jd.lowerAngle = -3.14*0.4f;
			jd.upperAngle = 3.14*0.4f;
			jd.collideConnected=collideKnees;
			m.joint =(b2RevoluteJoint*)m_world->CreateJoint(&jd);
			AddMotor(m);
		}

		// lower leg
		b2PolygonDef foot;
		foot.SetAsBox(0.8f, 0.4f);
		foot.density=1.0f;
		foot.filter.groupIndex = collideKnees ? 1 : -1;
		Motor f;
		b2BodyDef fd;
		fd.position=b2Vec2(start.x, .5f);
		b2Body* fb = m_world->CreateBody(&fd);
		fb->CreateShape(&foot);
		fb->SetMassFromShapes();
		b2RevoluteJointDef jd;
		if (hasKnee)
			jd.Initialize(body, fb, body->GetWorldCenter());
		else
			jd.Initialize(chassis, fb, start);

		jd.enableMotor=true;
		jd.maxMotorTorque=mmt;
		jd.enableLimit=false;
		jd.collideConnected=false;
		f.joint=(b2RevoluteJoint*)m_world->CreateJoint(&jd);

		AddMotor(f);
	}

	Walker(int legs, bool knees, bool limits=true, bool collide=false)
	{
		this->limits = limits;
		this->collideKnees = collide;
		hasKnee = knees;
		m_offset.Set(0.0f, 6.0f);

		float ld = 4.0f;
		float w = ld * legs;

		b2PolygonDef sd;
		sd.density = 0.2f;
		sd.SetAsBox(w*0.4f, 1.0f);
		sd.filter.groupIndex = 0;
		b2BodyDef bd;
		bd.position = m_offset;
		chassis = m_world->CreateBody(&bd);
		chassis->CreateShape(&sd);
		chassis->SetMassFromShapes();

		for(int x=0;x<legs;x++) {
			CreateLeg(b2Vec2(-w/2+ld*(x+0.5f), m_offset.y));
		}
	}

	void Step(TestSettings* settings) {
		UpdateMotors();

		Test::Step(settings);
	}

	void AddMotor(Motor m)
	{
		m.paramIndex = inputs.size();
		motors.push_back(m);

		ParamInfo inp (-100,100);

		for (int i=0;i<7;i++)
			inputs.push_back(inp);
	}

	void UpdateMotors()
	{
		if(params.empty())
			return;

		for(int i=0;i<motors.size();i++) {
			b2RevoluteJoint* j = motors[i].joint;
			float *param = &params[motors[i].paramIndex];
			float ang = j->GetJointAngle ();

			float wantedAng = param[0] + param[1] * sinf((param[2] * m_time - param[3]) * 0.1f) + param[4] * sinf(ang) + param[5] * chassis->GetAngle();
			float s = param[6];
			
			j->SetMotorSpeed( (ang > wantedAng) ? -s : s);
		}
	}


	float GetScore()
	{
		b2Vec2 p = chassis->GetWorldCenter();
		return p.x;
		//	return chassis->GetLinearVelocity().x;
	}

};


