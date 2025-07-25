#ifndef ACTION_H
#define ACTION_H


#include "DecisionTreeNode.h"
#include <string>
#include <iostream>
#include "Actor.h" // Ensure this is included for Actor references
#include "SceneManager.h"



class Action : public DecisionTreeNode {
    std::string actionName;
    float attackCooldown; // The cooldown duration in seconds
    float attackTimer;    // Tracks the time since the last attack

public:

    Actor* actor;         // The actor performing the action
    Actor* targetActor;   // The target actor (e.g., player)

    Action(const std::string& name, Actor* actor, Actor* targetActor, SceneManager* manager);
    DecisionTreeNode* makeDecision(float deltaTime);
    const std::string& GetActionName() const { return actionName; }
private:
    SceneManager* sceneManager;
};

#endif // ACTION_H
