#ifndef DECISION_TREE_NODE_H
#define DECISION_TREE_NODE_H


class DecisionTreeNode
{
public:
    virtual ~DecisionTreeNode() {}
    virtual DecisionTreeNode* makeDecision(float deltaTime) = 0;

};

#endif // DECISION_TREE_NODE_H
