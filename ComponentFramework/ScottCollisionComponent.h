#pragma once
#include "TransformComponent.h"

enum class ColliderType {
	Sphere,
	AABB
};

class CollisionComponent: public Component {
	friend class CollisionSystem;
    CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent(CollisionComponent&&) = delete;
	CollisionComponent& operator = (const CollisionComponent&) = delete;
	CollisionComponent& operator = (CollisionComponent&&) = delete;
protected:
	ColliderType colliderType;  
	float radius; /// sphere collision

public:
	CollisionComponent(Component* parent_, ColliderType type_, float radius_ );
	bool OnCreate(){return true;}
	void OnDestroy(){}
	void Update(const float deltaTime_){}
	void Render()const{}
};


