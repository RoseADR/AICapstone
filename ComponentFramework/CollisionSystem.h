#pragma once
#include <vector>
#include "CollisionComponent.h"
#include "PhysicsComponent.h"
#include "Actor.h"
#include "Debug.h"
#include <Sphere.h>
#include <unordered_set>
#include <chrono>

using namespace MATH;

struct CollisionState {
	bool isCurrentlyColliding = false;
	bool wasUpdatedThisFrame = false;
	
};

class CollisionSystem {
private:

//	std::unordered_map<Actor*, CollisionState> collisionStates;
	std::unordered_set<std::string> currentCollisions;


	std::vector<Ref<Actor>> collidingActors;
	Ref<Actor> factory;
	Ref<Actor> bridge;
	Ref<Actor> character;
	bool isGrounded = false;


public:
	/// This function will check the the actor being added is new and has the all proper components 
	void AddActor(Ref<Actor> actor_) {


		if (actor_->GetComponent<CollisionComponent>().get() == nullptr) {
			Debug::Error("The Actor must have a CollisionComponent - ignored ", __FILE__, __LINE__);
			return;
		}

		if (actor_->GetComponent<PhysicsComponent>().get() == nullptr && actor_->GetComponent<TransformComponent>().get() == nullptr) {
			Debug::Error("The Actor must have a PhysicsComponent or TransformComponent - ignored ", __FILE__, __LINE__);
			return;
		}

		collidingActors.push_back(actor_);
	}

	void SetFactory(Ref<Actor> factory_) { factory = factory_; }
	void SetBridge(Ref<Actor> bridge_) { bridge = bridge_; }
	void SetCharacter(Ref<Actor> character_) { character = character_; }

	bool SphereSphereCollisionDetection(const Sphere& s1, const Sphere& s2) const;
	void SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2, Ref<PhysicsComponent> pc2);

	bool SphereAABBCollisionDetection(const Sphere& s, const AABB& box) const;
	void SphereAABBCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, AABB bb2);

	bool SpherePlaneCollisionDetection( const Sphere& s1,const Plane& p1);
	void SpherePlaneCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Plane p2);

	void Update(const float deltaTime);


	
	
	
};
