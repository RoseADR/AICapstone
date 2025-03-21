#include "TreeBuilder.h"
#include "FloatDecision.h"
#include "tinyxml2.h"
#include <iostream>
#include <unordered_map>

using namespace tinyxml2;

void logNodeParsing(const std::string& id, const std::string& type) {
    std::cout << "[LOG]: Parsed Node: ID = " << id << ", Type = " << type << std::endl;
}

void logNodeLinking(const std::string& id, const std::string& trueNode, const std::string& falseNode) {
    std::cout << "[LOG]: Linked Node: ID = " << id
        << ", TrueNode = " << trueNode
        << ", FalseNode = " << falseNode << std::endl;
}

DecisionTreeNode* TreeBuilder::buildTree(const std::string& xmlFile, Actor* actor, Actor* targetActor) {
    XMLDocument doc;
    if (doc.LoadFile(xmlFile.c_str()) != XML_SUCCESS) {
        std::cerr << "[ERROR]: Failed to load XML file: " << xmlFile << std::endl;
        return nullptr;
    }

    XMLElement* rootNode = doc.FirstChildElement("DecisionTree")->FirstChildElement("Node");
    if (!rootNode) {
        std::cerr << "[ERROR]: Invalid XML structure: No root node found." << std::endl;
        return nullptr;
    }

    std::unordered_map<std::string, DecisionTreeNode*> nodeMap;

    // Parse all nodes
    for (XMLElement* xmlNode = rootNode; xmlNode; xmlNode = xmlNode->NextSiblingElement("Node")) {
        if (!buildNode(xmlNode, nodeMap, actor, targetActor)) {
            std::cerr << "[ERROR]: Failed to build node." << std::endl;
        }
    }

    // Link TrueNode and FalseNode for Decision nodes
    for (XMLElement* xmlNode = rootNode; xmlNode; xmlNode = xmlNode->NextSiblingElement("Node")) {
        const char* id = xmlNode->Attribute("id");
        if (!id) {
            std::cerr << "[ERROR]: Node is missing 'id' attribute." << std::endl;
            continue;
        }

        auto it = nodeMap.find(id);
        if (it == nodeMap.end()) {
            std::cerr << "[ERROR]: Node with ID '" << id << "' not found in nodeMap." << std::endl;
            continue;
        }

        auto* decision = dynamic_cast<Decision*>(it->second);
        if (decision) {
            const char* trueId = xmlNode->FirstChildElement("TrueNode") ?
                xmlNode->FirstChildElement("TrueNode")->Attribute("id") : nullptr;
            const char* falseId = xmlNode->FirstChildElement("FalseNode") ?
                xmlNode->FirstChildElement("FalseNode")->Attribute("id") : nullptr;

            if (!trueId) {
                std::cerr << "[ERROR]: Missing TrueNode ID for node: " << id << std::endl;
            }
            else if (nodeMap.find(trueId) == nodeMap.end()) {
                std::cerr << "[ERROR]: TrueNode with ID '" << trueId << "' not found in nodeMap." << std::endl;
            }
            else {
                decision->trueNode = nodeMap[trueId];
            }

            if (!falseId) {
                std::cerr << "[ERROR]: Missing FalseNode ID for node: " << id << std::endl;
            }
            else if (nodeMap.find(falseId) == nodeMap.end()) {
                std::cerr << "[ERROR]: FalseNode with ID '" << falseId << "' not found in nodeMap." << std::endl;
            }
            else {
                decision->falseNode = nodeMap[falseId];
            }

            logNodeLinking(id, trueId ? trueId : "None", falseId ? falseId : "None");
        }
    }

    return nodeMap.count("root") > 0 ? nodeMap["root"] : nullptr;
}

DecisionTreeNode* TreeBuilder::buildNode(XMLElement* xmlNode, std::unordered_map<std::string, DecisionTreeNode*>& nodeMap, Actor* actor,
    Actor* targetActor) {
    const char* type = xmlNode->Attribute("type");
    const char* id = xmlNode->Attribute("id");

    if (!type || !id) {
        std::cerr << "[ERROR]: Node is missing 'type' or 'id' attribute." << std::endl;
        return nullptr;
    }

    std::string nodeType(type);
    std::string nodeId(id);

    if (nodeType == "Action") {
        const char* actionName = xmlNode->FirstChildElement("Action") ?
            xmlNode->FirstChildElement("Action")->GetText() : nullptr;

        if (!actionName) {
            std::cerr << "[ERROR]: Action node '" << nodeId << "' is missing an Action name." << std::endl;
            return nullptr;
        }

        auto* action = new Action(
            actionName,
            actor,        // Enemy actor
            targetActor,  // Player actor
            SceneManager::GetInstance()  // Now you can use this
        );


        nodeMap[nodeId] = action;
        logNodeParsing(nodeId, "Action");
        return action;
    }

    if (nodeType == "Decision") {
        const char* decisionType = xmlNode->FirstChildElement("Condition") &&
            xmlNode->FirstChildElement("Condition")->FirstChildElement("Check") ?
            xmlNode->FirstChildElement("Condition")->FirstChildElement("Check")->Attribute("type") : nullptr;

        float value = xmlNode->FirstChildElement("Condition") &&
            xmlNode->FirstChildElement("Condition")->FirstChildElement("Check") ?
            xmlNode->FirstChildElement("Condition")->FirstChildElement("Check")->FloatAttribute("value") : 0.0f;

        if (!decisionType) {
            std::cerr << "[ERROR]: Decision node '" << nodeId << "' is missing a Condition type." << std::endl;
            return nullptr;
        }

        FloatDecision* decision = new FloatDecision(
            decisionType,
            0.0f,    // Default minValue
            value,   // Max value from XML
            actor,   // Enemy actor
            targetActor, // Player actor
            nullptr, // TrueNode placeholder
            nullptr  // FalseNode placeholder
        );
        nodeMap[nodeId] = decision;
        logNodeParsing(nodeId, "Decision");
        return decision;
    }

    std::cerr << "[ERROR]: Unknown node type '" << nodeType << "' for node: " << nodeId << std::endl;
    return nullptr;
}