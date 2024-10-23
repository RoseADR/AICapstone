#pragma once

#include "VMath.h"
#include "KinematicBodyComponent.h"
#include "SteeringOutputComponent.h"

class SteeringBehaviour
{
protected:
	SteeringOutput* result;
	const PhysicsComponent* npc;

public:
	SteeringBehaviour()
	{
		result = new SteeringOutput();
		npc = NULL;
	}

	virtual ~SteeringBehaviour()
	{
		if (result)
		{
			delete result;
		}
	}

	virtual SteeringOutput* getSteering() = 0;
	// abstract class: means every subclass must override the get steering

};