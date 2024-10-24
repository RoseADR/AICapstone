#pragma once

#include "MMath.h"
#include "VMath.h"
#include "Component.h"

using namespace MATH;

class PhysicsComponent : public Component 
{
    PhysicsComponent(const PhysicsComponent&) = delete;
    PhysicsComponent(PhysicsComponent&&) = delete;
    PhysicsComponent& operator = (const PhysicsComponent&) = delete;
    PhysicsComponent& operator = (PhysicsComponent&&) = delete;

protected:
        // inherited classes can access this
        Vec3 pos;
        Vec3 vel;
        Vec3 accel;
        float mass;
        float orientation;		// facing this direction
        float rotation;			// rotating at this speed (radians per second?)
        float angular;          // angular acceleration
        float maxSpeed;
        float maxAcceleration;
        float maxRotation;
        float maxAngular;
        float radius;           // for getting near walls

public:
        PhysicsComponent(Component* parent_);
        PhysicsComponent(
            Component* parent_,
            Vec3 pos_, Vec3 vel_, Vec3 accel_,
            float mass_,
            float radius_,
            float orientation_,
            float rotation_,
            float angular_,
            float maxSpeed_,
            float maxAcceleration_,
            float maxRotation_,
            float maxAngular_
        );
        virtual ~PhysicsComponent();
        virtual void Update(float deltaTime);
        void ApplyForce(Vec3 force_);
        Vec3 getPos() const { return pos; }
        Vec3 getVel() const { return vel; }
        Vec3 getAccel() const { return accel; }
        float getMass() const { return mass; }
        float getOrientation() const { return orientation; }
        float getRotation() const { return rotation; }
        float getAngular() const { return angular; }
        float getMaxSpeed() const { return maxSpeed; }
        float getMaxAcceleration() const { return maxAcceleration; }
        float getMaxRotation() const { return maxRotation; }
        float getMaxAngular() const { return maxAngular; }

        // Added this as public to deal with my demo 
        // so that mouse position can be copied into a Body.
        void setPos(Vec3 pos);

        bool OnCreate() {};
        void OnDestroy() {};
        void Render()const {};
};

