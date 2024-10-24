#pragma once
#include "Component.h"
#include "Matrix.h"
#include "QMath.h"
#include "Euler.h"

using namespace MATH;

class PhysicsComponent : public Component {
	PhysicsComponent(const PhysicsComponent&) = delete;
	PhysicsComponent(PhysicsComponent&&) = delete;
	PhysicsComponent& operator = (const PhysicsComponent&) = delete;
	PhysicsComponent& operator = (PhysicsComponent&&) = delete;

private:
	Vec3 pos;
	Vec3 scale;
	Vec3 vel;
	Vec3 accel;
	Quaternion orientation;
	float mass;
	//float rotation;			// rotating at this speed (radians per second?)
	//float angular;          // angular acceleration
	//float maxSpeed;
	//float maxAcceleration;
	//float maxRotation;
	//float maxAngular;
	//float radius;           // for getting near walls

 
public:
	PhysicsComponent(Component* parent_);
	PhysicsComponent(
		Component* parent_,
		Vec3 pos_, 
		Vec3 vel_, 
		Vec3 accel_, 
		float mass_,
		Quaternion orientation_, 
		Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f)

	);
	~PhysicsComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render() const;


	/// Inlined methods 
	Vec3 GetPos() { return pos; }
	Vec3 GetScale() { return scale; }
	Quaternion GetQuaternion() { return orientation; }
	float getMass() const { return mass; }
	Vec3 getVel() const { return vel; }
	Vec3 getAccel() const { return accel; }

	/*Vec3 getVel() const { return vel; }
	Vec3 getAccel() const { return accel; }
	float getMass() const { return mass; }
	Quaternion getOrientation() const { return orientation; }
	float getRotation() const { return rotation; }
	float getAngular() const { return angular; }
	float getMaxSpeed() const { return maxSpeed; }
	float getMaxAcceleration() const { return maxAcceleration; }
	float getMaxRotation() const { return maxRotation; }
	float getMaxAngular() const { return maxAngular; }*/

	void SetQuaternion(const Quaternion orientation_) { orientation = orientation_; }
	Matrix4 GetTransformMatrix() const;
	void SetPhysics(
		Vec3 pos_, 
		Vec3 vel_,
		Vec3 accel_,
		float mass_,
		const Quaternion orientation_, 
		const Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f)
	) 
	{
		pos = pos_;
		vel = vel_;
		accel = accel_;
		mass = mass_;
		orientation = orientation_;
		scale = scale_;
	}
	void SetPosition(const Vec3 pos_) { pos = pos_; }
	void SetVelocity(const Vec3 vel_) { vel = vel_; }

};

