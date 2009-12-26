#ifndef BIPED_H
#define BIPED_H

#include "Box2D.h"

#include <vector>

// Ragdoll class thanks to darkzerox.
class Biped
{
public:
	Biped(b2World*, const b2Vec2& position);
	~Biped();

	b2World* m_world;

	b2Body				*LFoot, *RFoot, *LCalf, *RCalf, *LThigh, *RThigh,
						*Pelvis, *Stomach, *Chest, *Neck, *Head,
						*LUpperArm, *RUpperArm, *LForearm, *RForearm, *LHand, *RHand;

	b2RevoluteJoint		*LAnkle, *RAnkle, *LKnee, *RKnee, *LHip, *RHip, 
						*LowerAbs, *UpperAbs, *LowerNeck, *UpperNeck,
						*LShoulder, *RShoulder, *LElbow, *RElbow, *LWrist, *RWrist;


	std::vector<b2RevoluteJoint*> getJoints();

};

#endif
