#include "Decision.h"

Decision::Decision(DecisionTreeNode* trueNode, DecisionTreeNode* falseNode)
{
}

DecisionTreeNode* Decision::getBranch()
{
    if (testValue()) {
        return trueNode;
    }
    else {
        return falseNode;
    }
}

DecisionTreeNode* Decision::makeDecision(float deltaTime)
{
    DecisionTreeNode* branch = getBranch();
    return branch->makeDecision(deltaTime);

}
