#include "Tile.h"
#include <glew.h>
#include "Scene1.h"

Tile::Tile(Node* node_, Vec3 pos_, float width_, float height_, Scene* scene_)
    : node{ node_ }, pos{ pos_ }, width{ width_ }, height{ height_ }, scene{ scene_ }, isPathTile{ false }, isDestination{ false }
{
}

void Tile::setGreen()
{
    isPathTile = true;
}

void Tile::setPathTile(bool isPath) {
    isPathTile = isPath;
}

void Tile::setDestinationTile(bool isDest) {
    isDestination = isDest;
}


void Tile::setColour() const {
    if (isDestination) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green for destination tile
    }
    else if (isPathTile) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green when hacked
    }
    else if (node && node->getIsBlocked()) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red for hacking required tiles
    }
    else {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue for normal tiles
    }
}


void Tile::render() const {
    glPushMatrix();

    // Apply the tile's model matrix
    glMultMatrixf(modelMatrix);

    // Set tile color based on state
    setColour();

    // Render the tile quad
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(width, 0.0f, 0.0f);
    glVertex3f(width, height, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    glEnd();

    // Render tile border
    glDisable(GL_DEPTH_TEST);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(width, 0.0f, 0.0f);
    glVertex3f(width, height, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    glEnd();
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
}


