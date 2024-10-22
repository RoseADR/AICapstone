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

void AiComponent::Update(const float deltaTime) {}
void AiComponent::Render()const {}
