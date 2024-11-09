#ifndef TILE_H
#define TILE_H

#include "Scene.h"
#include "Node.h"
#include "QMath.h"

using namespace MATH;

class Tile {
private:
    float width, height;
    Vec3 pos;
    Node* node;     // Associated node
    Scene* scene;
    bool isPathTile; // Indicates if this tile is part of the calculated path

public:
    Tile(Node* node_, Vec3 pos_, float width_, float height_, Scene* scene_);
    virtual ~Tile() {}

    Node* getNode() const { return node; }
    Vec3 getPosition() const { return pos; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

    // Declaration only; do not include a body here
    void setPathTile(bool isPath);

    void render() const;
};

#endif
