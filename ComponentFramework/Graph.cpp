#include "Graph.h"
#include <queue>
#include <cmath>
#include <algorithm>

Graph::Graph() {

}

Graph::~Graph() {

}

bool Graph::OnCreate(std::vector<Node*> nodes_) {
    int numNodes = nodes_.size();
    cost.resize(numNodes);

    for (int i = 0; i < numNodes; i++) {
        if (i != nodes_[i]->getLabel()) {
            std::cerr << "Node with label " << i << " not in the correct position.\n";
            return false;
        }
        node[i] = nodes_[i];
        cost[i].resize(numNodes, 0.0f);  // Initialize all costs to zero
    }

    return true;
}

int Graph::numNodes() {
    return node.size();
}

void Graph::addWeightedConnection(Node* fromNode, Node* toNode, float weight) {
    cost[fromNode->getLabel()][toNode->getLabel()] = weight;
}

std::vector<Node*> Graph::neighbours(Node* fromNode) {
    std::vector<Node*> result;
    int from = fromNode->getLabel();
    for (int j = 0; j < numNodes(); j++) {
        if (cost[from][j] > 0.0f) {
            result.push_back(getNode(j));
        }
    }
    return result;
}

struct NodeAndPriority {
    Node* node;
    float priority;
    bool operator()(NodeAndPriority const& lhs, NodeAndPriority const& rhs) {
        return lhs.priority > rhs.priority;
    }
};

float heuristic(Node* a, Node* b) {
    int gridSize = 8;  // Assume an 8x8 grid layout at the moment using the chess board

    // Calculate x, y coordinates of nodes in the grid based on their labels
    int ax = a->getLabel() % gridSize;
    int ay = a->getLabel() / gridSize;
    int bx = b->getLabel() % gridSize;
    int by = b->getLabel() / gridSize;

    // Manhattan distance for grid-based navigation
    return std::abs(ax - bx) + std::abs(ay - by);
}

std::vector<Node*> Graph::findPath(Node* startNode, Node* goalNode) {
    std::vector<Node*> result;

    // Priority queue for open nodes
    std::priority_queue<NodeAndPriority, std::vector<NodeAndPriority>, NodeAndPriority> frontier;
    startNode->setGCost(0.0f);
    startNode->setHCost(heuristic(startNode, goalNode));
    frontier.push({ startNode, startNode->getFCost() });

    // Track where each node came from to reconstruct the path
    std::map<int, Node*> came_from;
    came_from[startNode->getLabel()] = nullptr;

    while (!frontier.empty()) {
        Node* currentNode = frontier.top().node;
        frontier.pop();

        if (currentNode == goalNode) {
            // Reconstruct path by backtracking from goal to start
            while (currentNode != nullptr) {
                result.push_back(currentNode);
                currentNode = currentNode->getParent();
            }
            std::reverse(result.begin(), result.end());
            return result;
        }

        // Process each neighbor of the current node
        for (Node* neighbor : neighbours(currentNode)) {
            float new_cost = currentNode->getGCost() + cost[currentNode->getLabel()][neighbor->getLabel()];

            // Check if this path is better than any previous path to the neighbor
            if (new_cost < neighbor->getGCost()) {
                neighbor->setGCost(new_cost);
                neighbor->setHCost(heuristic(neighbor, goalNode));
                neighbor->setParent(currentNode);
                frontier.push({ neighbor, neighbor->getFCost() });
                came_from[neighbor->getLabel()] = currentNode;
            }
        }
    }

    return result;  // Returns an empty path if no path is found
}