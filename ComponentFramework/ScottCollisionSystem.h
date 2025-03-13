#pragma once
#include <vector>
#include "ScottCollisionComponent.h"
#include "PhysicsComponent.h"
#include "Actor.h"
#include "Debug.h"
#include <Sphere.h>

using namespace MATH;

class CollisionSystem {
private:

	std::vector<Ref<Actor>> collidingActors;
	Ref<Actor> factory;

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

	bool SphereSphereCollisionDetection(const Sphere& s1, const Sphere& s2) const;
	bool AABBAABBCollisionDetection(const AABB& bb1, const AABB& bb2) const;
	bool SphereAABBCollisionDetection(const Sphere& s1, const Sphere& s2) const;
	void SphereSphereCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Sphere s2, Ref<PhysicsComponent> pc2);
	void AABBAABBCollisionResponse(AABB bb1, Ref<PhysicsComponent> pc1, AABB bb2, Ref<PhysicsComponent> pc2);
	void SphereAABBCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, AABB bb2, Ref<PhysicsComponent> pc2);


	bool SpherePlaneCollisionDetection( const Sphere& s1,const Plane& p1);
	void SpherePlaneCollisionResponse(Sphere s1, Ref<PhysicsComponent> pc1, Plane p2/*, Ref<TransformComponent> tc2*/);

	void Update(const float deltaTime);

};