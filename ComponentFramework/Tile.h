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
    bool isPathTile;     // Indicates if this tile is part of the calculated path
    bool isDestination;  // Indicates if this tile is the destination tile

   

public:

    Matrix4 modelMatrix;  // Add model matrix to Tile


    Tile(Node* node_, Vec3 pos_, float width_, float height_, Scene* scene_);
    virtual ~Tile() {}

    Node* getNode() const { return node; }
    Vec3 getPosition() const { return pos; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

    void setPathTile(bool isPath);
    void setDestinationTile(bool isDest); // Add this function
    void setColour() const;

    void render() const;

    Matrix4 getModelMatrix() const { return modelMatrix; }  // Add getter for model matrix
};

#endif
