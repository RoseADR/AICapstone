//#pragma once
//#include "TransformComponent.h"
//
//enum class ColliderType {
//	Sphere,
//	AABB
//};
//
//class SFCollisionComponent: public Component {
//	friend class CollisionSystem;
//	SFCollisionComponent(const SFCollisionComponent&) = delete;
//	SFCollisionComponent(SFCollisionComponent&&) = delete;
//	SFCollisionComponent& operator = (const SFCollisionComponent&) = delete;
//	SFCollisionComponent& operator = (SFCollisionComponent&&) = delete;
//protected:
//	ColliderType colliderType;  
//	float radius; /// sphere collision
//
//public:
//	SFCollisionComponent(Component* parent_, ColliderType type_, float radius_ );
//	bool OnCreate(){return true;}
//	void OnDestroy(){}
//	void Update(const float deltaTime_){}
//	void Render()const{}
//};
//
//
