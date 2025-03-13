#include "AnimationComponent.h"

AnimationComponent::AnimationComponent(Component* parent_): Component(parent_)
{

}

AnimationComponent::~AnimationComponent()
{
	//OnDestroy();
}

void AnimationComponent::Update(const float deltaTime_)
{
	
}

void AnimationComponent::PlayAnimation(const char* AnimationName)
{
}

void AnimationComponent::StopAnimation()
{
}
