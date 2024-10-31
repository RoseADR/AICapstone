#include <iostream>
#include "PhysicsComponent.h"
#include "QMath.h"
#include "MMath.h"
using namespace MATH;

PhysicsComponent::PhysicsComponent(Component* parent_, Vec3 pos_, Quaternion orientation_,
	Vec3 vel_, Vec3 accel_, /*vec3 force*/ Vec3 scale_):TransformComponent(parent_) {
	
	// set values amateurishly makes it easier to read
	pos = pos_;
	vel = vel_;
	accel = accel_;
	// force
	orientation = orientation_;
	scale = scale_;

}


bool PhysicsComponent::OnCreate() {
	if (isCreated) return isCreated;
	isCreated = true;
	return isCreated;
}


void PhysicsComponent::Update(const float deltaTime) {
	
	pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime;
	vel += accel * deltaTime;

}

void PhysicsComponent::Render() const {

}


PhysicsComponent::~PhysicsComponent() {
	OnDestroy();
	isCreated = false;
}

void PhysicsComponent::OnDestroy() {
	isCreated = false;
}