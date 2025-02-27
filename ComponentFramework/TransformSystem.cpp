#include "TransformSystem.h"
#include "TransformSystem.h"
#include <iostream>

void TransformSystem::Update(float deltaTime) {
    for (auto& actor : transformActors) {
        Ref<TransformComponent> tc = actor->GetComponent<TransformComponent>();
        if (tc) {
            tc->Update(deltaTime);
        }

        Ref<CollisionComponent> cc = actor->GetComponent<CollisionComponent>();
        // Ensure planes are never affected by physics
        if (cc->GetColliderType() == ColliderType::PLANE) {
            tc->GetPosition(); 
        }
    }
}

