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

#ifndef BIPED_TEST_H
#define BIPED_TEST_H

#include "Biped.h"

class BipedTest : public Test
{
public:

	BipedTest()
	{
		m_biped = new Biped(m_world, b2Vec2(0.0f, 4.0f));

		std::vector<b2RevoluteJoint*> joints =  m_biped->getJoints();
	}

	~BipedTest()
	{
		delete m_biped;
	}

	static Test* Create()
	{
		return new BipedTest;
	}

	float GetScore() {
		return m_biped->Head->GetPosition().y;
	}

	Biped* m_biped;
};

#endif
