#pragma once

#include "Actor.h"

class CollisionOnlyActor : public Actor {
public:
	CollisionOnlyActor(Component* parent, const Vec3& pos, const Quaternion& rot, const Vec3& scale)
		: Actor(parent)
	{
		Ref<TransformComponent> tc = std::make_shared<TransformComponent>(this, pos, rot, scale);
		Ref<CollisionComponent> cc = std::make_shared<CollisionComponent>(this, ColliderType::AABB);
		cc->SetAABB(pos, rot, scale.x, scale.y, scale.z);
		AddComponent(tc);
		AddComponent(cc);

	}

	void Render() {}

};