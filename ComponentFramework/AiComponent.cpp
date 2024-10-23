#include "AiComponent.h"


AiComponent::AiComponent(Component* parent_) :
	Component(parent_) {

}

bool AiComponent::OnCreate() {
	if (isCreated) return isCreated;
	return isCreated;
}

AiComponent::~AiComponent() {
	OnDestroy();
}

void AiComponent::OnDestroy() {
}


void AiComponent::Update(const float deltaTime) {

}


void AiComponent::Render()const {}

Vec3 AiComponent::Follow(const Vec3 myLocation, Vec3 otherLocation){

	Vec3 enemy = myLocation;
	Vec3 character = otherLocation;

	Vec3 linear = character - enemy;
	linear = VMath::normalize(linear);

	return linear;

	std::cout << "Im following you" << std::endl;
	myLocation.print();
}

Vec3 AiComponent::Flee(const Vec3 myLocation, Vec3 otherLocation) {

	Vec3 enemy = myLocation;
	Vec3 character = otherLocation;

	Vec3 linear = character + enemy;
	linear = VMath::normalize(linear);

	return linear;

	std::cout << "Im following you" << std::endl;
	myLocation.print();
}

//
//
//
//void AiComponent::Seek(const PhysicsComponent* npc_, const PhysicsComponent* target_)
//{
//    npc = npc_;
//    target = target_;
//
//    getSeekSteering();
//}
//
//
//void AiComponent::Flee(const PhysicsComponent* npc_, const PhysicsComponent* target_)
//{
//    npc = npc_;
//    target = target_;
//    result = new SteeringOutput();
//
//    getFleeSteering();
//}
//
//void AiComponent::Arrive(const PhysicsComponent* npc_, const PhysicsComponent* target_)
//{
//   // npc = npc_;
//   // target = target_;
//    targetRadius = 1.0f;       // Distance where we consider the NPC has "arrived"
//    slowRadius = 10.0f;        // Distance to start slowing down
//    timeToTarget = 0.1f;       // Time over which to achieve speed
//   // result = new SteeringOutput();
//
//    getArriveSteering();
//
//}
//
//void AiComponent::getSeekSteering()
//{
//    // get direction to target
//    result->linear = target->getPos() - npc->getPos();
//
//    //angular accel in that direction
//    result->linear = VMath::normalize(result->linear) * npc->getMaxAcceleration();
//    result->angular = 0.0f;
//
//    return result;
//}
//
//void AiComponent::getFleeSteering()
//{
//    // get direction to target
//    result->linear = target->getPos() + npc->getPos();
//
//    //angular accel in that direction
//    result->linear = VMath::normalize(result->linear) * npc->getMaxAcceleration();
//    result->angular = 0.0f;
//
//    return result;
//}
//
//void AiComponent::getArriveSteering()
//{
//    // Get the direction to the target
//    Vec3 direction = target->getPos() - npc->getPos();
//    float distance = VMath::mag(direction);
//    // Check if the character has arrived at the target
//    if (distance < targetRadius) {
//        // If within the target radius, no need for steering
//        delete result;
//    }
//
//    // Determine the target speed based on distance
//    float targetSpeed;
//    if (distance > slowRadius) {
//        // If outside the slow radius, use maximum speed
//        targetSpeed = npc->getMaxAcceleration();
//    }
//    else {
//        // Use a scaled speed based on the distance within the slow radius
//        targetSpeed = npc->getMaxAcceleration() * (distance / slowRadius);
//    }
//
//    // The target velocity is a combination of speed and direction
//    Vec3 targetVelocity = VMath::normalize(direction) * targetSpeed;
//
//    // Calculate the linear acceleration needed to reach the target velocity
//    result->linear = (targetVelocity - npc->getVel()) / timeToTarget;
//
//    // Clip the acceleration if it's too high
//    if (VMath::mag(result->linear) > npc->getMaxAcceleration()) {
//        result->linear = VMath::normalize(result->linear) * npc->getMaxAcceleration();
//    }
//
//    // No angular steering is applied
//    result->angular = 0.0f;
//
//    // Return the calculated steering output
//    return result;
//}
// 
//
