#pragma once
#include <glew.h>
#include "Component.h"
#include "PhysicsComponent.h"
#include "SteeringBehaviourComponent.h"

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