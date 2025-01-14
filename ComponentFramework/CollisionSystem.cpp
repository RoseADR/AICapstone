#include "CollisionSystem.h"
#include "TransformComponent.h"
#include <iostream>

bool CollisionHandler::CheckCollision(const std::shared_ptr<Actor>& actor1, const std::shared_ptr<Actor>& actor2) {
    auto transform1 = actor1->GetComponent<TransformComponent>();
    auto transform2 = actor2->GetComponent<TransformComponent>();

    if (!transform1 || !transform2) return false;

    // AABB Collision Detection
    Vec3 pos1 = transform1->GetPosition();
    Vec3 pos2 = transform2->GetPosition();

    // Adjust based on the size of the actors if needed
    float distance = VMath::mag(pos1 - pos2);


    if (distance < 1.0f) { // Assuming a threshold for collision
        std::cout << "touch" << std::endl;
        return true;
    }
    return false;
}
