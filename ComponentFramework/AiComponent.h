#pragma once
#include <glew.h>
#include "Component.h"
<<<<<<< HEAD
#include "Actor.h"
#include "VMath.h"

using namespace MATH;
=======
#include "PhysicsComponent.h"
#include "SteeringBehaviourComponent.h"
>>>>>>> 95b0d356e0a66608a34e613007354971c98cb159

class AiComponent : public Component {
	AiComponent(const AiComponent&) = delete;
	AiComponent(AiComponent&&) = delete;
	AiComponent& operator=(const AiComponent&) = delete;
	AiComponent& operator=(AiComponent&&) = delete;
public:
	AiComponent(Component* parent_);
	virtual ~AiComponent();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime_);
	virtual void Render()const;

<<<<<<< HEAD
	void Follow(const Vec3 location);

};
=======

	class SteeringBehaviour {
	protected:
		SteeringOutput* result;
		const PhysicsComponent* target;
		const PhysicsComponent* npc;

		// for ariive
		float targetRadius; // Radius for arriving at the target
		float slowRadius;   // Radius for slowing down
		float timeToTarget; // Time over which to achieve speed
	public:
		void Seek(const PhysicsComponent* npc_, const PhysicsComponent* target_);
		void Flee(const PhysicsComponent* npc_, const PhysicsComponent* target_);
		void Arrive(const PhysicsComponent* npc_, const PhysicsComponent* target_);

		SteeringOutput* getSeekSteering();
		SteeringOutput* getFleeSteering();
		SteeringOutput* getArriveSteering();

	};

};
>>>>>>> 95b0d356e0a66608a34e613007354971c98cb159
