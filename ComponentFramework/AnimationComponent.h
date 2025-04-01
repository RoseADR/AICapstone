#pragma once
#include "Component.h"
#include "Actor.h"

class AnimationComponent : public Component
{
	friend class AnimationSystem;

	AnimationComponent(const AnimationComponent&) = delete;
	AnimationComponent(AnimationComponent&&) = delete;
	AnimationComponent& operator=(const AnimationComponent&) = delete;
	AnimationComponent& operator=(AnimationComponent&&) = delete;

private:
	

protected:
	Actor* actor;
	const char* currentAnimation;
	float currentTime;
	float animationSpeed;
	bool isPlaying;
	bool isLooping;
	Vec2 index;

public:
	AnimationComponent(Component* parent_);
	~AnimationComponent();

	void Update(const float deltaTime_) override;
	void PlayAnimation(const char* AnimationName);
	void StopAnimation();
};

