#ifndef ACTION_H
#define ACTION_H


#include "DecisionTreeNode.h"
#include <iostream>

class Action : public DecisionTreeNode {
    std::string actionName;

public:
    explicit Action(const std::string& name);
    DecisionTreeNode* makeDecision() override;
};

#endif // ACTION_H
