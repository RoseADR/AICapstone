#include "AnimationSystem.h"

void AnimationSystem::Update(const float deltaTime){

	for (auto& actor : animationActors) {
		Ref<AnimationComponent> ac = actor->GetComponent<AnimationComponent>();
		
	}

}
