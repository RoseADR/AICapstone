#include "TreeBuilder.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include "tinyxml2.h"

using namespace tinyxml2;

DecisionTreeNode* parseNode(XMLElement* xmlNode, std::unordered_map<std::string, DecisionTreeNode*>& nodeMap) {
    const char* type = xmlNode->Attribute("type");
    const char* id = xmlNode->Attribute("id");

    if (!type || !id) return nullptr;

    std::string nodeType(type);
    std::string nodeId(id);

    if (nodeType == "Action") {
        const char* actionName = xmlNode->FirstChildElement("Action")->GetText();
        auto* action = new Action(actionName ? actionName : "Unnamed Action");
        nodeMap[nodeId] = action;
        return action;
    }

    if (nodeType == "Decision") {
        float minValue = xmlNode->FirstChildElement("Condition")
            ->FirstChildElement("Check")->FloatAttribute("minValue");
        float maxValue = xmlNode->FirstChildElement("Condition")
            ->FirstChildElement("Check")->FloatAttribute("maxValue");
        const char* trueId = xmlNode->FirstChildElement("TrueNode")
            ->FirstChildElement("Node")->Attribute("id");
        const char* falseId = xmlNode->FirstChildElement("FalseNode")
            ->FirstChildElement("Node")->Attribute("id");

        FloatDecision* decision = new FloatDecision(minValue, maxValue, 0.0f, nullptr, nullptr);
        nodeMap[nodeId] = decision;

        decision->trueNode = nullptr; // Placeholder, will be set after all nodes are parsed
        decision->falseNode = nullptr; // Placeholder

        return decision;
    }

    return nullptr;
}

DecisionTreeNode* TreeBuilder::buildTree(const std::string& xmlFile) {
    XMLDocument doc;
    if (doc.LoadFile(xmlFile.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load XML file: " << xmlFile << std::endl;
        return nullptr;
    }

    XMLElement* rootNode = doc.FirstChildElement("DecisionTree")->FirstChildElement("Node");
    if (!rootNode) {
        std::cerr << "Invalid XML structure: No root node found." << std::endl;
        return nullptr;
    }

    std::unordered_map<std::string, DecisionTreeNode*> nodeMap;

    // Parse all nodes
    for (XMLElement* xmlNode = rootNode; xmlNode; xmlNode = xmlNode->NextSiblingElement("Node")) {
        parseNode(xmlNode, nodeMap);
    }

    // Link TrueNode and FalseNode
    for (auto& [id, node] : nodeMap) {
        auto* decision = dynamic_cast<Decision*>(node);
        if (decision) {
            const char* trueId = doc.FirstChildElement("DecisionTree")
                ->FirstChildElement("Node")->FirstChildElement("TrueNode")
                ->FirstChildElement("Node")->Attribute("id");
            const char* falseId = doc.FirstChildElement("DecisionTree")
                ->FirstChildElement("Node")->FirstChildElement("FalseNode")
                ->FirstChildElement("Node")->Attribute("id");

            decision->trueNode = nodeMap[trueId];
            decision->falseNode = nodeMap[falseId];
        }
    }

    return nodeMap["root"];
}