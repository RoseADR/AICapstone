#pragma once

#include "VMath.h"
#include "KinematicBodyComponent.h"
#include "SteeringOutputComponent.h"

class SteeringBehaviour : public Component
{
	SteeringBehaviour(const SteeringBehaviour&) = delete;
	SteeringBehaviour(SteeringBehaviour&&) = delete;
	SteeringBehaviour& operator = (const SteeringBehaviour&) = delete;
	SteeringBehaviour& operator = (SteeringBehaviour&&) = delete;

	bool OnCreate() {};
	void OnDestroy() {};
	void Update(const float deltaTime_) {};
	void Render()const {};


protected:
	SteeringOutput* result;
	Ref<PhysicsComponent> npc;

public:

	SteeringBehaviour(Component* parent_) :Component(parent_) {};
		/*{
			result = new SteeringOutput();
			npc = NULL;
		}*/

		~SteeringBehaviour();
	/*{
		if (result)
		{
			delete result;
		}
	}*/

	virtual SteeringOutput* getSteering() = 0;
	// abstract class: means every subclass must override the get steering


	
};