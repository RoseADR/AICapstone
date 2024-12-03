#ifndef TREE_BUILDER_H
#define TREE_BUILDER_H

#include <string>
#include <unordered_map>
#include "DecisionTreeNode.h"
#include "tinyxml2.h"
#include "Action.h"

using namespace tinyxml2;

class TreeBuilder {
public:
    static DecisionTreeNode* buildTree(const std::string& xmlFile, Actor* actor, Actor* targetActor);

private:
    static DecisionTreeNode* buildNode(XMLElement* xmlNode, std::unordered_map<std::string, DecisionTreeNode*>& nodeMap, Actor* actor,
        Actor* targetActor);
};

#endif // TREE_BUILDER_H


