#pragma once
#include <glew.h>
#include "Component.h"
#include "Actor.h"
#include "VMath.h"

using namespace MATH;

#include "PhysicsComponent.h"


class AiComponent : public Component {
	AiComponent(const AiComponent&) = delete;
	AiComponent(AiComponent&&) = delete;
	AiComponent& operator=(const AiComponent&) = delete;
	AiComponent& operator=(AiComponent&&) = delete;
protected:

	Ref<PhysicsComponent> target; 
	Ref<PhysicsComponent> npc;

	// for ariive
	float targetRadius; // Radius for arriving at the target
	float slowRadius;   // Radius for slowing down
	float timeToTarget; // Time over which to achieve speed

public:
	

	AiComponent(Component* parent_);
	virtual ~AiComponent();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime_);
	virtual void Render()const;


	Vec3 Seek(const Vec3 myLocation, Vec3 otherLocation);

	Vec3 Flee(const Vec3 myLocation, Vec3 otherLocation);

	Vec3 Pursuit(const Vec3 myLocation, const Vec3 otherLocation, const Vec3 otherVelocity); //added the otherVelocity parameter which is the players velocity needed in the calculation

	Vec3 Arrive(const Vec3 myLocation, Vec3 otherLocation);

			
	
		};

