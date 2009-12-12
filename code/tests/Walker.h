#pragma once

//#include "../Box2D/Examples/TestBed/Tests/TheoJansen.h"


class Walker : public Test
{
public:
	b2Vec2 m_offset;
	b2Body* chassis;
	bool hasKnee, limits, collideKnees;

	float GetScore()
	{
		b2Vec2 p = chassis->GetWorldCenter();
		return p.x;// p.y * 100 + p.x;
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
		b2Body* body;
		
		if (hasKnee) {
			body = m_world->CreateBody(&upperDef);
			b2CircleDef cd;
			cd.density=1.0f;
			cd.radius=1.0f;
			cd.filter.groupIndex=-1;
			body->CreateShape(&cd);
			body->SetMassFromShapes();

			b2RevoluteJointDef jd;
			jd.Initialize(chassis, body, start);
			jd.enableMotor=true;
			jd.maxMotorTorque=mmt;
			jd.enableLimit=limits;
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
		foot.filter.groupIndex = -1;
		Motor f;
		b2BodyDef fd;
		fd.position=b2Vec2(start.x, 0.0f);
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
		jd.enableLimit=limits;
		jd.collideConnected=collideKnees;
		f.joint=(b2RevoluteJoint*)m_world->CreateJoint(&jd);

		AddMotor(f);
	}

	Walker(int legs, bool knees, bool limits=true, bool collide=false)
	{
		this->limits = limits;
		this->collideKnees = collide;
		hasKnee = knees;
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
			sd.filter.groupIndex = 0;
			b2BodyDef bd;
			bd.position = m_offset;
			chassis = m_world->CreateBody(&bd);
			chassis->CreateShape(&sd);
			chassis->SetMassFromShapes();
		}

		for(int x=0;x<legs;x++) {
			CreateLeg(b2Vec2(-w/2+ld*(x+0.5f), m_offset.y));
		}
	}
};


