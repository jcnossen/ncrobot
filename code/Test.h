/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef TEST_H
#define TEST_H

#include <Box2D.h>
#include "Render.h"

class Test;
struct TestSettings;

class TestFactoryBase {
	TestFactoryBase* next;
	static TestFactoryBase* list;

public:
	TestFactoryBase();
	static std::vector<TestFactoryBase*> getFactoryList();

	virtual int getNumTests() = 0;
	virtual const char* getTestName(int i) = 0;
	virtual Test* createTest(int i) = 0;
};

template<typename T>
class TestFactory : public TestFactoryBase {
	const char *name;
public:
	TestFactory(const char *n) { name=n; }
	int getNumTests() { return 1; }
	const char* getTestName(int i) { return name; }
	Test* createTest(int i) { return new T; }
};

struct TestSettings
{
	TestSettings() :
		hz(60.0f),
		iterationCount(10),
		drawStats(0),
		drawShapes(1),
		drawJoints(1),
		drawCoreShapes(0),
		drawAABBs(0),
		drawOBBs(0),
		drawPairs(0),
		drawContactPoints(0),
		drawContactNormals(0),
		drawContactForces(0),
		drawFrictionForces(0),
		drawCOMs(0),
		enableWarmStarting(1),
		enablePositionCorrection(1),
		enableTOI(1),
		pause(0),
		singleStep(0),
		optimizing(false),
		motorCtl(1)
		{}

	float32 hz;
	int32 iterationCount;
	int32 drawShapes;
	int32 drawJoints;
	int32 drawCoreShapes;
	int32 drawAABBs;
	int32 drawOBBs;
	int32 drawPairs;
	int32 drawContactPoints;
	int32 drawContactNormals;
	int32 drawContactForces;
	int32 drawFrictionForces;
	int32 drawCOMs;
	int32 drawStats;
	int32 enableWarmStarting;
	int32 enablePositionCorrection;
	int32 enableTOI;
	int32 pause;
	int32 singleStep;
	bool optimizing; // is PSO running, in this case do not draw anything
	int32 motorCtl;

	bool drawing() { return optimizing==false; }
};

struct TestEntry {
	TestFactoryBase* factory;
	const char *name;
	int index;

	Test* create() { return factory->createTest(index); }
};

// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class DestructionListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Shape* shape) { B2_NOT_USED(shape); }
	void SayGoodbye(b2Joint* joint);

	Test* test;
};

class BoundaryListener : public b2BoundaryListener	
{
public:
	void Violation(b2Body* body);

	Test* test;
};

const int32 k_maxContactPoints = 2048;

class ContactListener : public b2ContactListener
{
public:
	void Add(const b2ContactPoint* point);
	void Persist(const b2ContactPoint* point);
	void Remove(const b2ContactPoint* point);

	Test* test;
};

enum ContactState
{
	e_contactAdded,
	e_contactPersisted,
	e_contactRemoved,
};

struct ContactPoint
{
	b2Shape* shape1;
	b2Shape* shape2;
	b2Vec2 normal;
	b2Vec2 position;
	b2Vec2 velocity;
	b2ContactID id;
	ContactState state;
};

struct ParamInfo
{
	ParamInfo(float min_, float max_, const char* name_=0){ 
		min=min_;
		max=max_;
		name=name_;
	}

	float min, max;
	const char* name;
};


class Test
{
public:

	Test();
	virtual ~Test();

	void SetTextLine(int32 line) { textLine = line; }
	virtual void Step(TestSettings* settings);

	void DrawMouseJoint();
	void DrawContactPoints( TestSettings* settings );
	virtual void Keyboard(unsigned char key) { B2_NOT_USED(key); }
	void MouseDown(const b2Vec2& p);
	void MouseUp();
	void MouseMove(const b2Vec2& p);
	void SetupListeners();

	std::vector<ParamInfo> GetParamInfo();

	virtual std::string GetInfo() { return ""; }

	void SetControlParams(float* vals);

	void CreateBaseWorld();

	virtual float GetScore() = 0;
	void SetupForPSO();

	float GetTime() { return time; }
	void SetTime(float t) { time=t; }

	// Let derived tests know that a joint was destroyed.
	virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }
	virtual void BoundaryViolated(b2Body* body) { B2_NOT_USED(body); }

	uint CalcHash();

	static bool IsBodyTouching(b2Body *a, b2Body *b);
	static bool IsShapeTouching(b2Shape* a, b2Shape *b);

protected:
	friend class DestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	b2AABB worldAABB;
	ContactPoint contactPoints[k_maxContactPoints];
	int32 contactPointCount;
	DestructionListener destructionListener;
	BoundaryListener boundaryListener;
	ContactListener contactListener;
	DebugDraw debugDraw;
	int32 textLine;
	b2World* world;
	b2MouseJoint* mouseJoint;
	float time;
	b2Shape* mainGroundShape;

	std::vector<float> params;
	std::vector<ParamInfo> inputs;
};

#endif
