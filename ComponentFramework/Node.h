#ifndef NODE_H
#define NODE_H

#include <limits>

class Node
{
private:
    int label;
    float gCost;  // Movement cost from the start node
    float hCost;  // Heuristic cost to the goal node
    float fCost;  // Total cost (gCost + hCost)
    Node* parent; // Pointer to parent node for path reconstruction
    bool isBlocked; // To indicate if the node is blocked

public:
    Node(int label_) : label(label_), gCost(std::numeric_limits<float>::infinity()),
        hCost(0.0f), fCost(0.0f), parent(nullptr), isBlocked(false) {};
    ~Node() {}

    int getLabel() const { return label; }
    float getGCost() const { return gCost; }
    float getHCost() const { return hCost; }
    float getFCost() const { return fCost; }
    Node* getParent() const { return parent; }

    void setGCost(float g) { gCost = g; updateFCost(); }
    void setHCost(float h) { hCost = h; updateFCost(); }
    void setParent(Node* p) { parent = p; }

    void resetCosts() {
        gCost = std::numeric_limits<float>::infinity();
        hCost = 0.0f;
        fCost = 0.0f;
        parent = nullptr;
    }

    void updateFCost() { fCost = gCost + hCost; } // Recalculate total cost


    bool getIsBlocked() const { return isBlocked; }
    void setIsBlocked(bool blocked) { isBlocked = blocked; }
};

#endif