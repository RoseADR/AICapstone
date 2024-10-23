#ifndef STEERING_OUTPUT_COMPONENT_H
#define STEERING_OUTPUT_COMPONENT_H

#include "Vector.h"
#include "Component.h"


using namespace MATH;

class SteeringOutput : public Component
{
SteeringOutput(const SteeringOutput&) = delete;
SteeringOutput(SteeringOutput&&) = delete;
SteeringOutput& operator = (const SteeringOutput&) = delete;
SteeringOutput& operator = (SteeringOutput&&) = delete;

	// Yes these are public! 
	// The steering algorithms will be creating instances and then setting the values.
	// There might be a better way, but I can't think of it right now.


Vec3 linear;
float angular;

inline const SteeringOutput& operator + (const SteeringOutput& v) const
{ return SteeringOutput (nullptr, linear + v.linear, angular + v.angular); }

inline SteeringOutput& operator += (const SteeringOutput& v) 
{
	linear += v.linear;
	angular += v.angular;
	return *this;
}

	SteeringOutput(Component* parent_);
	~SteeringOutput();
	SteeringOutput(Component* parent_, Vec3 linear_, float angular_);
	
	bool OnCreate() {};
	void OnDestroy() {};
	void Update(const float deltaTime_) {};
	void Render()const {};

	// This class does not have any behaviours, just constructors!

};
#endif