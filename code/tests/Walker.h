#pragma once

//#include "../Box2D/Examples/TestBed/Tests/TheoJansen.h"


class Walker : public Test
{
protected:
	struct Motor {
		b2RevoluteJoint* joint;
		int paramIndex;
	};

	float groundHitPenalty;
	std::vector<Motor> motors;

	class ContactListener : public b2ContactListener {
	public:
		Walker* walker;

 		void Add(const b2ContactPoint* point) { B2_NOT_USED(point); }
 		void Persist(const b2ContactPoint* point) { B2_NOT_USED(point); }
// 		void Remove(const b2ContactPoint* point) { B2_NOT_USED(point); }
		void Result(const b2ContactResult* point);
	};
	ContactListener contactListener;
	bool isHittingGround;

public:

	b2Vec2 m_offset;
	b2Body* chassis;

	void CreateLeg(b2Vec2 start, bool knee, bool limits);
	void CreateLegs(int n, bool knees, bool limits);
	b2Body* CreateLegPiece(b2Body* parent, b2Vec2 pos, float len, float angle);

	Walker(float w);

	std::string GetInfo();

	void Step(TestSettings* settings);
	void AddMotor(Motor m);
	void UpdateMotors();

	float GetScore();

};


