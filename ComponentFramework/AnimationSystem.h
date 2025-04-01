#pragma once
#include <vector>
#include "AnimationComponent.h"
#include "Actor.h"
#include "Debug.h"

class AnimationSystem
{
private:
	std::vector<Ref<Actor>> animationActors;

public:

	void AddActor(Ref<Actor> actor_) {
		animationActors.push_back(actor_);
	}

	Ref<AnimationComponent> GetActor() const {
		for (auto component : animationActors) {
			if (dynamic_cast<PhysicsComponent*>(component.get())) {
				/// This is a dynamic cast designed for shared_ptr's
				/// https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
				return std::dynamic_pointer_cast<AnimationComponent>(component);

			}
		}
		return Ref<AnimationComponent>(nullptr);
	}


	void Update(const float deltaTime);

};

