#include "StdIncl.h"

#include "Test.h"
#include "Walker.h"


#define MOTOR_TORQUE 1200
#define USE_LIMITS true
#define RESTITUTION 0.0f
#define FRICTION 0.6f

class WalkerTestFactory : TestFactoryBase {
public:
	int getNumTests() { return 6; }

	const char* getTestName(int i) { 
		const char *names[] = {
			"1-lp walker (knees)",
			"2-lp walker (knees)",
			"3-lp walker (knees)",
			"4-lp walker (knees)",
			"4-lp walker",
			"crab walker"
		};

		return names[i];
	}

	Test* createTest(int i) {

		if (i == 5) {
			Walker* crab = new Walker(1.5f);

			b2Vec2 s = crab->m_offset;
			crab->CreateLegPiece(crab->chassis, s+b2Vec2(-2.0f, 0.0f), 6, -PI/2);

			return crab;
		}
		else {
			Walker *w = new Walker(2.0f * (i+1));

			switch (i) {
				case 0: w->CreateLegs(1, true, true); break;
				case 1: w->CreateLegs(2, true, true);break;
				case 2: w->CreateLegs(3, true, true);break;
				case 3: w->CreateLegs(4, true, true);break;
				case 4: w->CreateLegs(4, false, true);break;
			}

			return w;
		}
	}
};

static WalkerTestFactory walkerTestFactory;



void Walker::CreateLeg( b2Vec2 start, bool knee, bool limits )
{
	float kneeY = m_offset.y*0.4f;
	float limitAng=3.1415*0.5f;
	// upper leg, distance joint to chassis

	bool collideKnees=false;

	b2Body* b = chassis;
	b2Vec2 attachPos = b2Vec2(start.x, start.y);
	
	for(int k=0;k<1;k++) {
		b = CreateLegPiece(b, attachPos, 6.0f, 0.0f);//b->GetAngle() + start.x*0.1f);
		attachPos = b->GetWorldCenter();
	}

	// lower leg
	b2PolygonDef foot;
	foot.SetAsBox(0.8f, 0.4f);
	foot.density=0.7f;
	foot.friction=FRICTION;
	foot.restitution=RESTITUTION;
	foot.filter.groupIndex = collideKnees ? 1 : -1;
	Motor f;
	b2BodyDef fd;
	fd.position=attachPos - b2Vec2(0.0f, 4.0f);
	b2Body* fb = world->CreateBody(&fd);
	fb->CreateShape(&foot);
	fb->SetMassFromShapes();
	b2RevoluteJointDef jd;
	jd.Initialize(b, fb, attachPos);

	jd.enableMotor=true;
	jd.maxMotorTorque=MOTOR_TORQUE;
	jd.lowerAngle = -limitAng;
	jd.upperAngle = limitAng;
	jd.enableLimit=USE_LIMITS;
	jd.collideConnected=true;
	f.joint=(b2RevoluteJoint*)world->CreateJoint(&jd);

	AddMotor(f);
}


Walker::Walker(float w)
{
	m_offset.Set(0.0f, 12.0f);
	groundHitPenalty=0.0f;

	contactListener.walker=this;
	world->SetContactListener(&contactListener);

	b2PolygonDef sd;
	sd.density = 1.5f;
	sd.SetAsBox(w, 1.0f);
	sd.filter.groupIndex = 1;
	sd.friction=FRICTION;
	sd.restitution=RESTITUTION;
	b2BodyDef bd;
	bd.position = m_offset;
	chassis = world->CreateBody(&bd);
	chassis->CreateShape(&sd);
	chassis->SetMassFromShapes();
//	chassis->SetLinearVelocity(b2Vec2(100,0));
}

void Walker::Step( TestSettings* settings )
{
	UpdateMotors();

	isHittingGround=false;
	Test::Step(settings);

	if (isHittingGround)
		groundHitPenalty += (1.0f / settings->hz);
}

void Walker::AddMotor( Motor m )
{
	m.paramIndex = inputs.size();
	motors.push_back(m);

	ParamInfo inp (-100,100);

	for (int i=0;i<7;i++)
		inputs.push_back(inp);
}

void Walker::UpdateMotors()
{
	if(params.empty())
		return;

	for(int i=0;i<motors.size();i++) {
		b2RevoluteJoint* j = motors[i].joint;
		float *param = &params[motors[i].paramIndex];
		float ang = j->GetJointAngle ();

		float wantedAng = param[0] + param[1] * sinf(param[2] * time - param[3]) + param[4] * sinf(ang) + param[5] * chassis->GetAngle();
		float s = param[6];

		//j->SetMotorSpeed( (ang > wantedAng) ? -s : s);
		j->SetMotorSpeed(wantedAng);
	}
}

float Walker::GetScore()
{
	b2Vec2 p = chassis->GetWorldCenter();
	return p.x- groundHitPenalty*5.0f;
	//	return chassis->GetLinearVelocity().x;
}

void Walker::CreateLegs( int n, bool knees, bool limits )
{
	float lw = 5.0f;
	float w=lw*n;

	for (int i=0;i<n;i++)
		CreateLeg(b2Vec2(m_offset.x-w/2+lw*(i+0.5f), m_offset.y), knees, limits);
}

void Walker::ContactListener::Result(const b2ContactResult* point) {

	if ( (point->shape1 == walker->mainGroundShape && point->shape2 == walker->chassis->GetShapeList()) ||
		(point->shape2 == walker->mainGroundShape && point->shape1 == walker->chassis->GetShapeList()) )
		walker->isHittingGround=true;
}

std::string Walker::GetInfo()
{
	return SPrintf("GroundHitPenalty: %.2f", groundHitPenalty);
}

b2Body* Walker::CreateLegPiece( b2Body* parent, b2Vec2 pos, float len, float angle )
{
	float limitAng=PI*0.5f;

	Vector2 v = Vector2(angle-0.5f*PI) * len;
	b2BodyDef bd;
	bd.position = pos + b2Vec2(v.x, v.y);
	bd.angularDamping=0.5f;
	bd.linearDamping=0.5f;

	b2Body* body = world->CreateBody(&bd);
	b2CircleDef cd;
	cd.density=0.5f;
	cd.radius=.5f;
	cd.friction=FRICTION;
	cd.restitution=RESTITUTION;
	cd.filter.groupIndex=-1;
	body->CreateShape(&cd);
	body->SetMassFromShapes();

	b2RevoluteJointDef jd;
	jd.Initialize(parent, body, pos);
	jd.enableMotor=true;
	jd.maxMotorTorque= MOTOR_TORQUE;
	jd.enableLimit= USE_LIMITS;
	jd.lowerAngle = -limitAng;
	jd.upperAngle = limitAng;
	jd.collideConnected=true;
	Motor m;
	m.joint =(b2RevoluteJoint*)world->CreateJoint(&jd);
	AddMotor(m);
	
	return body;
}

