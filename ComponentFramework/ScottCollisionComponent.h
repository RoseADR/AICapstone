#pragma once
#include "TransformComponent.h"
#include "ScottCollisionComponent.h"
#include <cmath>

enum class ColliderType {
	Sphere,
	AABB,
	PLANE,
	RECTANGLE
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

/*** Other definitions might be:
struct AABB {
	Vec3 min; /// Bottom, left, front
	Vec3 max; /// Top, right, back
};

struct AABB {
	Vec3 min; /// Bottom, left, front
	float dx,dy,dz;
};
***/

class CollisionComponent : public Component {
	friend class CollisionSystem;
	CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent(CollisionComponent&&) = delete;
	CollisionComponent& operator = (const CollisionComponent&) = delete;
	CollisionComponent& operator = (CollisionComponent&&) = delete;
protected:
	ColliderType colliderType;
	float radius; /// sphere collision


public:
	CollisionComponent(Component* parent_, ColliderType type_, float radius_);
	bool OnCreate() { return true; }
	void OnDestroy() {}
	void Update(const float deltaTime_) {}
	void Render()const {}
};

