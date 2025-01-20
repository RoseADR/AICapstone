#include "CollisionSystem.h"
#include "TransformComponent.h"
#include <iostream>

bool CollisionHandler::CheckCollision(const std::shared_ptr<Actor>& actor1, const std::shared_ptr<Actor>& actor2) {
    auto transform1 = actor1->GetComponent<TransformComponent>();
    auto transform2 = actor2->GetComponent<TransformComponent>();

    if (!transform1 || !transform2) return false;

    Vec3 pos1 = transform1->GetPosition();
    Vec3 pos2 = transform2->GetPosition();

    // Define AABB boundaries for the plane (floor)
    Vec3 floorMin = pos2 - Vec3(5.0f, 5.0f, 11.25f); // Adjust size as needed
    Vec3 floorMax = pos2 + Vec3(5.0f, 3.0f, 11.25f);

    // Check if the character is within the floor's boundaries
    if (pos1.x >= floorMin.x && pos1.x <= floorMax.x &&
        pos1.y >= floorMin.y && pos1.y <= floorMax.y &&
        pos1.z >= floorMin.z && pos1.z <= floorMax.z) {
        return true;
    }

    return false;
}
void CollisionHandler::ResolveCollision(const std::shared_ptr<Actor>& character, const std::shared_ptr<Actor>& floor) {
    auto transform1 = character->GetComponent<TransformComponent>();
    auto transform2 = floor->GetComponent<TransformComponent>();

    if (!transform1 || !transform2) return;

    Vec3 pos1 = transform1->GetPosition(); // Character position
    Vec3 pos2 = transform2->GetPosition(); // Floor position

    // Define AABB boundaries for the floor
    Vec3 floorMin = pos2 - Vec3(4.0f, 5.0f, 5.0f); // Adjust floor bounds to match the actual plane
    Vec3 floorMax = pos2 + Vec3(4.0f, 5.0f, 5.0f);

    // Check if character is within the floor's bounds
    if (pos1.x >= floorMin.x && pos1.x <= floorMax.x &&
        pos1.z >= floorMin.z && pos1.z <= floorMax.z) {
        // Align the character's Y position to the floor's surface
        pos1.y = floorMax.y; // Set Y to the top surface of the plane
        transform1->SetPosition(pos1);
    }
}


