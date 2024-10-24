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

Vec3 AiComponent::Seek(const Vec3 myLocation, Vec3 otherLocation){

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

	Vec3 linear =  enemy - character;
	linear = VMath::normalize(linear);

	return linear;

	std::cout << "Im not following you" << std::endl;
	myLocation.print();
}

Vec3 AiComponent::Pursuit(const Vec3 myLocation, const Vec3 otherLocation, const Vec3 otherVelocity) {

	Vec3 enemy = myLocation;
	Vec3 character = otherLocation;
	Vec3 velocity = otherVelocity;

    // Calculate the direction to the target (other actor)
	Vec3 direction = character - enemy;

    // Distance between NPC (self) and target (other actor)
    float distance = VMath::mag(direction);

	float speed = VMath::mag(enemy);//(npc->getVel());
    // Maximum prediction time
    float maxPrediction = 5.0f;
    float prediction;

    // If the NPC is slower than the distance / maxPrediction, use maxPrediction
    if (speed < distance / maxPrediction) {
        prediction = maxPrediction;
    }
    else {
        prediction = distance / speed;
    }

	Vec3 pursuit = character;
	pursuit += velocity * prediction;

	return pursuit;

    // Predict the future position of the target (other actor)
    //Vec3 futurePos = otherVelocity * prediction + otherLocation;

    // Call Seek to pursue the predicted position
   // return (myLocation, futurePos);
}

Vec3 AiComponent::Arrive(const Vec3 myLocation, Vec3 otherLocation)
{


	Vec3 enemy = myLocation;
	Vec3 character = otherLocation;
	Vec3 velocity;
	float targetRadius = 5.0f;
	float slowRadius = 5.0f;
	float maxAccel = 1.0f;
	float timeToTarget = 0.1f;

	Vec3 linear = enemy - character;
	linear = VMath::normalize(linear);

	// Get the direction to the target
	Vec3 direction = character - enemy;
	float distance = VMath::mag(direction);
	// Check if the character has arrived at the target

	//if (distance < targetRadius) {
	//	// If within the target radius, no need for steering
	//	delete linear;
	//}

	// Determine the target speed based on distance
	float targetSpeed;
	if (distance > slowRadius) {
		// If outside the slow radius, use maximum speed
		targetSpeed = 1.0f;
	}
	else {
		// Use a scaled speed based on the distance within the slow radius
		targetSpeed = (distance / slowRadius);
	}

	// The target velocity is a combination of speed and direction
	Vec3 targetVelocity = VMath::normalize(direction) * targetSpeed;

	// Calculate the linear acceleration needed to reach the target velocity
	linear = (targetVelocity - velocity) / timeToTarget;

	// Clip the acceleration if it's too high
	if (VMath::mag(linear) > targetSpeed) {
		linear = VMath::normalize(linear) * maxAccel;
	}

	// No angular steering is applied
	//result->angular = 0.0f;

	// Return the calculated steering output
	return linear;




	//Vec3 direction = character - enemy;
	//float distance = VMath::mag(direction);

	////check if we are there, return no steering
	//if (distance < targetRadius) {
	//	return nullptr;
	//}

	//// if we are outside the slow radius, use max speed
	//if (distance > slowRadius) {
	//	targetSpeed = maxSpeed;
	//}
	//else {
	//	//use scaled speed
	//	targetSpeed = maxSpeed * distance / slowRadius;
	//}

	//// te target velocity combines speed and direction
	//targetVelocity = VMath::normalize(direction);
	//targetVelocity *= targetSpeed;

	////Acceleration tries to get to the target velocity
	//Vec3 accel = targetVelocity - 


	
	return Vec3(0,0,0);
}

