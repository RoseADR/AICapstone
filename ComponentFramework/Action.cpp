#include "Action.h"

Action::Action(const std::string& name) : actionName(name) {}

DecisionTreeNode* Action::makeDecision() {
    std::cout << "Executing action: " << actionName << std::endl;
    return this;
}