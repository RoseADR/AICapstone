#pragma once
#include "TransformComponent.h"
#include "Matrix.h"
//#include "QMath.h"
//#include "Euler.h"

using namespace MATH;

class PhysicsComponent : public TransformComponent {
	friend class PhysicsSystem;
	friend class CollisionSystem;

	PhysicsComponent(const PhysicsComponent&) = delete;
	PhysicsComponent(PhysicsComponent&&) = delete;
	PhysicsComponent& operator = (const PhysicsComponent&) = delete;
	PhysicsComponent& operator = (PhysicsComponent&&) = delete;

private:

protected:
	Vec3 vel;
	Vec3 accel;
	Vec3 force;
	float mass;
	bool useGravity;
	//float rotation;			// rotating at this speed (radians per second?)
	//float angular;          // angular acceleration
	//float maxSpeed;
	//float maxAcceleration;
	//float maxRotation;
	//float maxAngular;
	//float radius;           // for getting near walls

 
public:
	PhysicsComponent(
		Component* parent_, Vec3 pos_, Quaternion orientation_,
		Vec3 vel_ = Vec3(0.0f, 0.0f, 0.0f), Vec3 accel_ = Vec3(0.0f, 0.0f, 0.0f),
		Vec3 force_ = Vec3(0.0f,0.0f,0.0f), Vec3 scale = Vec3(1.0f, 1.0f, 1.0f)
	);
	~PhysicsComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render() const;


	/// Inlined methods 
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
	float getMaxAngular() const { return maxAngular; }
	*/

	void ApplyForce(const Vec3 force_);
	void UndoForce();

	void SetPhysics(Vec3 vel_, Vec3 accel_, float mass_ = 1.0f) 
	{
		vel = vel_;
		accel = accel_;
		mass = mass_;
	}
	void SetVelocity(const Vec3 vel_) { vel = vel_; }
	void SetAccel(const Vec3 accel_) { accel = accel_; }

};
