#include "SteeringOutputComponent.h"

SteeringOutput::SteeringOutput(Component* parent_) :Component(parent_)
{
	linear = Vec3(0, 0, 0);
	angular = 0.0f;
}

SteeringOutput::~SteeringOutput()
{
}

SteeringOutput::SteeringOutput(Component* parent_, Vec3 linear_, float angular_) :Component(parent_)
{
	linear = linear_;
	angular = angular_;
}
