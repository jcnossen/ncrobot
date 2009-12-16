#include "StdIncl.h"

#include "Test.h"
#include "Walker.h"



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
//			crab->CreateLeg( b2Vec2(s.x-1.0f, s.y), true, )

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
	float mmt=1200.0f;
	float kneeY = m_offset.y*0.4f;
	float limitAng=3.1415*0.5f;
	// upper leg, distance joint to chassis

	limits=false;
	bool collideKnees=true;

	b2BodyDef upperDef;
	upperDef.position = b2Vec2(start.x, start.y*0.5f);

	Motor m;
	b2Body* body;

	if (knee) {
		body = world->CreateBody(&upperDef);
		b2CircleDef cd;
		cd.density=1.0f;
		cd.radius=1.0f;
		cd.filter.groupIndex=collideKnees?1:-1;
		body->CreateShape(&cd);
		body->SetMassFromShapes();

		b2RevoluteJointDef jd;
		jd.Initialize(chassis, body, start);
		jd.enableMotor=true;
		jd.maxMotorTorque=mmt;
		jd.enableLimit=limits;
		jd.lowerAngle = -limitAng;
		jd.upperAngle = limitAng;
		jd.collideConnected=true;
		m.joint =(b2RevoluteJoint*)world->CreateJoint(&jd);
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
	b2Body* fb = world->CreateBody(&fd);
	fb->CreateShape(&foot);
	fb->SetMassFromShapes();
	b2RevoluteJointDef jd;
	if (knee)
		jd.Initialize(body, fb, body->GetWorldCenter());
	else
		jd.Initialize(chassis, fb, start);

	jd.enableMotor=true;
	jd.maxMotorTorque=mmt;
	jd.lowerAngle = -limitAng;
	jd.upperAngle = limitAng;
	jd.enableLimit=limits;
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
	sd.density = 0.2f;
	sd.SetAsBox(w, 1.0f);
	sd.filter.groupIndex = 0;
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

		j->SetMotorSpeed( (ang > wantedAng) ? -s : s);
	}
}

float Walker::GetScore()
{
	b2Vec2 p = chassis->GetWorldCenter();
	return p.x;// - groundHitPenalty*10.0f;
	//	return chassis->GetLinearVelocity().x;
}

void Walker::CreateLegs( int n, bool knees, bool limits )
{
	float lw = 4.0f;
	float w=lw*n;

	for (int i=0;i<n;i++)
		CreateLeg(b2Vec2(m_offset.x-w/2+lw*(i+0.5f), m_offset.y-1.5f), knees, limits);
}

void Walker::ContactListener::Result(const b2ContactResult* point) {

	if ( (point->shape1 == walker->mainGroundShape && point->shape2 == walker->chassis->GetShapeList()) ||
		(point->shape2 == walker->mainGroundShape && point->shape1 == walker->chassis->GetShapeList()) )
		walker->isHittingGround=true;
}

std::string Walker::GetInfo() {
	return SPrintf("GroundHitPenalty: %.2f", groundHitPenalty);
}
