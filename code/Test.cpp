/*
ncrobot
Natural Computing Robot
This app is based on the TestBed example application from Box2D (by Erin Catto)

Copyright 2009 Jelmer Cnossen, particle swarm optimization code by Johan de Ruiter
License (ZLIB):

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/
#include "StdIncl.h"
#include "Test.h"
#include "Render.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#define APIENTRY
#else
#include "freeglut/GL/glut.h"
#endif

#include <cstdio>

//#include "../Box2D/Examples/TestBed/Tests/TheoJansen.h"

TestFactoryBase* TestFactoryBase::list = 0;

TestFactoryBase::TestFactoryBase()
{
	next = list;
	list = this;
}

std::vector<TestFactoryBase*> TestFactoryBase::getFactoryList()
{
	std::vector<TestFactoryBase*> v;
	for(TestFactoryBase* p = list; p; p = p->next)
		v.push_back(p);
	return v;
}

void Test::SetupForPSO()
{
	world->SetDebugDraw(0);
	world->SetDestructionListener(0);
	world->SetBoundaryListener(0);
	world->SetContactFilter(0);
}

std::vector<ParamInfo> Test::GetParamInfo()
{
	return inputs;
}

void Test::SetControlParams(float* v)
{
	if(params.empty())
		params.resize(inputs.size());
	params=std::vector<float>(v,v+params.size());
}

void Test::CreateBaseWorld()
{
	b2PolygonDef sd;
	float e=300.0f;
	sd.SetAsBox(e, 10.0f);
	sd.friction = 1.0f;

	b2BodyDef bd;
	bd.position.y=-10.0f;
	b2Body* ground = world->CreateBody(&bd);
	ground->CreateShape(&sd);
	mainGroundShape = ground->GetShapeList();

	sd.SetAsBox(0.5f, 5.0f, b2Vec2(-e, 15.0f), 0.0f);
	ground->CreateShape(&sd);

	sd.SetAsBox(0.5f, 5.0f, b2Vec2(e, 15.0f), 0.0f);
	ground->CreateShape(&sd);
}



void DestructionListener::SayGoodbye(b2Joint* joint)
{
	if (test->mouseJoint == joint)
	{
		test->mouseJoint = NULL;
	}
	else
	{
		test->JointDestroyed(joint);
	}
}

void BoundaryListener::Violation(b2Body* body)
{
}

void ContactListener::Add(const b2ContactPoint* point)
{
	if (test->contactPointCount == k_maxContactPoints)
	{
		return;
	}

	ContactPoint* cp = test->contactPoints + test->contactPointCount;
	cp->shape1 = point->shape1;
	cp->shape2 = point->shape2;
	cp->position = point->position;
	cp->normal = point->normal;
	cp->id = point->id;
	cp->state = e_contactAdded;

	++test->contactPointCount;
}

void ContactListener::Persist(const b2ContactPoint* point)
{
	if (test->contactPointCount == k_maxContactPoints)
	{
		return;
	}

	ContactPoint* cp = test->contactPoints + test->contactPointCount;
	cp->shape1 = point->shape1;
	cp->shape2 = point->shape2;
	cp->position = point->position;
	cp->normal = point->normal;
	cp->id = point->id;
	cp->state = e_contactPersisted;

	++test->contactPointCount;
}

void ContactListener::Remove(const b2ContactPoint* point)
{
	if (test->contactPointCount == k_maxContactPoints)
	{
		return;
	}

	ContactPoint* cp = test->contactPoints + test->contactPointCount;
	cp->shape1 = point->shape1;
	cp->shape2 = point->shape2;
	cp->position = point->position;
	cp->normal = point->normal;
	cp->id = point->id;
	cp->state = e_contactRemoved;

	++test->contactPointCount;
}

Test::Test()
{
	worldAABB.lowerBound.Set(-2000.0f, -1000.0f);
	worldAABB.upperBound.Set(2000.0f, 2000.0f);
	b2Vec2 gravity;
	gravity.Set(0.0f, -40.0f);
	bool doSleep = false;
	world = new b2World(worldAABB, gravity, doSleep);
	textLine = 30;
	mouseJoint = NULL;
	contactPointCount = 0;
	time=0.0f;

	CreateBaseWorld();
}

void Test::SetupListeners()
{
	destructionListener.test = this;
	boundaryListener.test = this;
//	contactListener.test = this;
	world->SetDestructionListener(&destructionListener);
	world->SetBoundaryListener(&boundaryListener);
//	world->SetContactListener(&contactListener);
	world->SetDebugDraw(&debugDraw);
}

Test::~Test()
{
	// By deleting the world, we delete the bomb, mouse joint, etc.
	delete world;
	world = NULL;
}

void Test::MouseDown(const b2Vec2& p)
{
	if (mouseJoint != NULL)
	{
		return;
	}

	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	// Query the world for overlapping shapes.
	const int32 k_maxCount = 10;
	b2Shape* shapes[k_maxCount];
	int32 count = world->Query(aabb, shapes, k_maxCount);
	b2Body* body = NULL;
	for (int32 i = 0; i < count; ++i)
	{
		b2Body* shapeBody = shapes[i]->GetBody();
		if (shapeBody->IsStatic() == false && shapeBody->GetMass() > 0.0f)
		{
			bool inside = shapes[i]->TestPoint(shapeBody->GetXForm(), p);
			if (inside)
			{
				body = shapes[i]->GetBody();
				break;
			}
		}
	}

	if (body)
	{
		b2MouseJointDef md;
		md.body1 = world->GetGroundBody();
		md.body2 = body;
		md.target = p;
#ifdef TARGET_FLOAT32_IS_FIXED
		md.maxForce = (body->GetMass() < 16.0)? 
			(1000.0f * body->GetMass()) : float32(16000.0);
#else
		md.maxForce = 1000.0f * body->GetMass();
#endif
		mouseJoint = (b2MouseJoint*)world->CreateJoint(&md);
		body->WakeUp();
	}
}

void Test::MouseUp()
{
	if (mouseJoint)
	{
		world->DestroyJoint(mouseJoint);
		mouseJoint = NULL;
	}
}

void Test::MouseMove(const b2Vec2& p)
{
	if (mouseJoint)
	{
		mouseJoint->SetTarget(p);
	}
}

void Test::Step(TestSettings* settings)
{
	float32 timeStep = settings->hz > 0.0f ? 1.0f / settings->hz : float32(0.0f);

	if (settings->pause)
	{
		if (settings->singleStep)
			settings->singleStep = 0;
		else
			timeStep = 0.0f;

		if (settings->drawing()) 
			DrawString(5, 30, "****PAUSED****");
	}

	uint32 flags = 0;
	flags += settings->drawShapes			* b2DebugDraw::e_shapeBit;
	flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
	flags += settings->drawCoreShapes	* b2DebugDraw::e_coreShapeBit;
	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
	flags += settings->drawOBBs				* b2DebugDraw::e_obbBit;
	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
	debugDraw.SetFlags(flags);

	world->SetWarmStarting(settings->enableWarmStarting > 0);
	world->SetPositionCorrection(settings->enablePositionCorrection > 0);
	world->SetContinuousPhysics(settings->enableTOI > 0);

	contactPointCount = 0;

	world->Step(timeStep, settings->iterationCount);
	world->Validate();

	if (mouseJoint && settings->drawing())
		DrawMouseJoint();

	if (settings->drawContactPoints && settings->drawing())
		DrawContactPoints(settings);

	time += timeStep;
}

template<typename T>
static void CalcStructHash(T& o, uint &hash)
{
	char *str = (char*)&o;
	int len = sizeof(T);

	unsigned int b = 378551;
	unsigned int a = 63689;

	for(uint i = 0; i < len; str++, i++)
	{
		hash = hash * a + (*str);
		a    = a * b;
	}
}

// Calculate a hash based on the body state
uint Test::CalcHash()
{
	uint hash = 0;

	for (b2Body* b =  world->GetBodyList(); b;b=b->GetNext()) {
		b2XForm xf = b->GetXForm();
		CalcStructHash(xf, hash);

		float32 av = b->GetAngularVelocity();
		b2Vec2 v = b->GetLinearVelocity();

		CalcStructHash(av, hash);
		CalcStructHash(v, hash);
	}

	return hash;
}

void Test::DrawContactPoints( TestSettings* settings )
{
	//const float32 k_impulseScale = 0.1f;
	const float32 k_axisScale = 0.3f;

	for (int32 i = 0; i < contactPointCount; ++i)
	{
		ContactPoint* point = contactPoints + i;

		if (point->state == 0)
		{
			// Add
			DrawPoint(point->position, 10.0f, b2Color(0.3f, 0.95f, 0.3f));
		}
		else if (point->state == 1)
		{
			// Persist
			DrawPoint(point->position, 5.0f, b2Color(0.3f, 0.3f, 0.95f));
		}
		else
		{
			// Remove
			DrawPoint(point->position, 10.0f, b2Color(0.95f, 0.3f, 0.3f));
		}

		if (settings->drawContactNormals == 1)
		{
			b2Vec2 p1 = point->position;
			b2Vec2 p2 = p1 + k_axisScale * point->normal;
			DrawSegment(p1, p2, b2Color(0.4f, 0.9f, 0.4f));
		}
		else if (settings->drawContactForces == 1)
		{
			//b2Vec2 p1 = point->position;
			//b2Vec2 p2 = p1 + k_forceScale * point->normalForce * point->normal;
			//DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
		}

		if (settings->drawFrictionForces == 1)
		{
			//b2Vec2 tangent = b2Cross(point->normal, 1.0f);
			//b2Vec2 p1 = point->position;
			//b2Vec2 p2 = p1 + k_forceScale * point->tangentForce * tangent;
			//DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
		}
	}
}

void Test::DrawMouseJoint()
{
	b2Body* body = mouseJoint->GetBody2();
	b2Vec2 p1 = body->GetWorldPoint(mouseJoint->m_localAnchor);
	b2Vec2 p2 = mouseJoint->m_target;

	glPointSize(4.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();
	glPointSize(1.0f);

	glColor3f(0.8f, 0.8f, 0.8f);
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();
}

bool Test::IsBodyTouching( b2Body *a, b2Body *b )
{
	for(b2Shape* sA = a->GetShapeList(); sA; sA=sA->GetNext())
		for(b2Shape* sB=b->GetShapeList(); sB;sB=sB->GetNext())
			if (IsShapeTouching(sA,sB)) return true;

	return false;
}

bool Test::IsShapeTouching( b2Shape* a, b2Shape *b )
{
	return false;
}

