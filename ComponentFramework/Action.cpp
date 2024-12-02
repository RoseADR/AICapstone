#include "Action.h"

Action::Action(const std::string& name) : actionName(name) {}

DecisionTreeNode* Action::makeDecision() {
    std::cout << "Executing action: " << actionName << std::endl;
    return this;
}

//#include "Condition.h"
//#include "VMath.h"
////#include "Character.h"
//
//using namespace MATH;
//
//
//Condition::Condition()
//{
//}
//
//ConditionOutOfRange::ConditionOutOfRange()
//{}
//
//bool ConditionOutOfRange::test(Actor* npc)
//{
//    float threshold = 2.0f;
//    if (VMath::distance(character->GetComponent<PhysicsComponent>()->GetPosition(), npc->GetComponent<PhysicsComponent>()->GetPosition()) > threshold)
//    {
//        return true;
//    }
//
//    return false;
//}
//
//ConditionInRange::ConditionInRange()
//{
//}
//
//bool ConditionInRange::test(Actor* npc)
//{
//    float threshold = 2.0f;
//    if (VMath::distance(character->GetComponent<PhysicsComponent>()->GetPosition(), npc->GetComponent<PhysicsComponent>()->GetPosition()) <= threshold)
//    {
//        return true;
//    }
//
//    return false;
//}
//
//
