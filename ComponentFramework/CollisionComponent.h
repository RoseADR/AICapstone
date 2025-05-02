#pragma once
#include "TransformComponent.h"
#include "CollisionComponent.h"
#include <cmath>
#include <Plane.h>
#include <PMath.h>
#include "Actor.h"

using namespace MATHEX;

enum class ColliderType {
	Sphere,
	PLANE,
	AABB
};

/// Definition of a Sphere and Plane are in the Math library

/// For an axis aligned bounding box, their are many ways to define the box.
/// My favorite way, the easiest to understand way, is this way.
/// Pick the center location of the box, then specify the radius from that center in 
/// the x, y,and z dimensions.

struct AABB {
	Vec3 center;
	float rx, ry, rz;
};

//// other definitions might be:
//struct AABB {
//	Vec3 min; /// Bottom, left, front
//	Vec3 max; /// Top, right, back
//	Vec3 size;       // Dimensions for AABB/OBB
//	Vec3 offset;     // Offset from parent center
//	
//};

//struct AABB {
//	Vec3 min; /// Bottom, left, front
//	float dx,dy,dz;
//};


class CollisionComponent : public Component {
	friend class CollisionSystem;
	CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent(CollisionComponent&&) = delete;
	CollisionComponent& operator = (const CollisionComponent&) = delete;
	CollisionComponent& operator = (CollisionComponent&&) = delete;
protected:
	TransformComponent* transform;
	ColliderType colliderType;
	float radius; /// sphere collision
	Vec3 normal;
	float dist;
	AABB aabb;
	


public:
	CollisionComponent(Component* parent_, ColliderType type_, 
		float radius_ = 0.0f, 
		Vec3 normal_ = Vec3(0.0f, 0.0f, 0.0f),
		float dist_ = 0.0f);

	bool OnCreate() { return true; }
	void OnDestroy() {}
	void Update(const float deltaTime_) {}
	void Render()const {}
	
	
	float GetRadius() const { return radius; }
	ColliderType GetColliderType() const { return colliderType; }

	void SetAABB(const Vec3& position, const Quaternion& rotation, float rx_, float ry_, float rz_);

	AABB GetAABB() const { return aabb; }


	//AABB AddAABBCollisionBox(const Vec3& pos, const Quaternion& rot, const Vec3& scale);

	void SetRadiusFromScale(const Vec3& scale) {
		radius = std::max({ scale.x, scale.y, scale.z }) * 0.5f;
	}

	void DrawAABB() const;
};

