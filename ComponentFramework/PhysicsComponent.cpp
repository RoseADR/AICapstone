#include <iostream>
#include "PhysicsComponent.h"
#include "QMath.h"
#include "MMath.h"
using namespace MATH;

PhysicsComponent::PhysicsComponent(Component* parent_):Component(parent_) {
	pos = Vec3(0.0f, 0.0f, 0.0f);
	vel = Vec3(0.0f, 0.0f, 0.0f);
	accel = Vec3(0.0f, 0.0f, 0.0f);
	mass = 1.0f;
	orientation = Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f));

	scale = Vec3(1.0f, 1.0f, 1.0f);

}

PhysicsComponent::PhysicsComponent(Component* parent_, 
	Vec3 pos_, 
	Vec3 vel_, 
	Vec3 accel_, 
	float mass_, 
	Quaternion orientation_, 
	Vec3 scale_)
	:
	Component{ parent_ }, 
	pos{ pos_ }, 
	vel{ vel_ }, 
	accel{ accel_ }, 
	mass{ mass_ }, 
	orientation {orientation_ }, 
	scale{ scale_ }
{
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

Matrix4 PhysicsComponent::GetTransformMatrix() const {
	return MMath::translate(pos) * MMath::scale(scale) * MMath::toMatrix4(orientation);
}

PhysicsComponent::~PhysicsComponent() {
	OnDestroy();
	isCreated = false;
}

void PhysicsComponent::OnDestroy() {
	isCreated = false;
}