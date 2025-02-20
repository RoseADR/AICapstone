#pragma once
#include <vector>
#include "TransformComponent.h"
#include "Actor.h"
#include "Debug.h"

class TransformSystem {
private:
	//const float gravity = -9.81f;
	std::vector<Ref<Actor>> transformActors;

	//CollisionSystem* collisionSystem;

public:

	void AddActor(Ref<Actor> actor_) {
		transformActors.push_back(actor_);
	}

	Ref<TransformComponent> GetActor() const {
		for (auto component : transformActors) {
			if (dynamic_cast<TransformComponent*>(component.get())) {
				/// This is a dynamic cast designed for shared_ptr's
				/// https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
				return std::dynamic_pointer_cast<TransformComponent>(component);

			}
		}
		return Ref<TransformComponent>(nullptr);
	}


	void Update(const float deltaTime);
};
