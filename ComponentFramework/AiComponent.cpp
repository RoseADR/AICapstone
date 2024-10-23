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

void AiComponent::Update(const float deltaTime) {

}
void AiComponent::Render()const {}

void AiComponent::Follow(const Vec3 location){

	Actor* me = dynamic_cast<Actor*>(parent);
	Vec3 mePos = me->GetComponent<TransformComponent>()->GetPosition();

	std::cout << "Im following you" << std::endl;
	location.print();
}
