#include "FloatDecision.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"

FloatDecision::FloatDecision(const std::string& type, float min, float max,
    Actor* actor, Actor* targetActor,
    DecisionTreeNode* trueNode, DecisionTreeNode* falseNode)
    : Decision(trueNode, falseNode), minValue(min), maxValue(max), type(type), actor(actor), targetActor(targetActor) {}

bool FloatDecision::testValue() const {
    if (type == "PlayerHealth") {
        // Check health of the player (targetActor)
        return targetActor->health > minValue;

    }
    else if (type == "PlayerFacingEnemy") {
        // Check if the player (targetActor) is facing the enemy (actor)
        auto transform1 = targetActor->GetComponent<TransformComponent>();
        auto transform2 = actor->GetComponent<TransformComponent>();
        if (!transform1 || !transform2) return false;

        // Calculate forward vector from quaternion
        Vec3 forward = QMath::rotate(Vec3(0.0f, 0.0f, 1.0f), transform1->GetQuaternion());
        forward = VMath::normalize(forward);

        // Vector to enemy actor
        Vec3 toActor = VMath::normalize(transform2->GetPosition() - transform1->GetPosition());

        // Dot product determines if the player is facing the enemy
        return VMath::dot(forward, toActor) > 0.0f;

    }
    else if (type == "PlayerDistance") {
        // Check distance between the player (targetActor) and enemy (actor)
        auto transform1 = targetActor->GetComponent<TransformComponent>();
        auto transform2 = actor->GetComponent<TransformComponent>();
        if (!transform1 || !transform2) return false;

        // Calculate distance and compare to maxValue
        float distance = VMath::distance(transform1->GetPosition(), transform2->GetPosition());
        return distance < maxValue;
    }

    return false; // Default case
}