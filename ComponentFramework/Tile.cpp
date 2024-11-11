#include "Tile.h"
#include <glew.h>

Tile::Tile(Node* node_, Vec3 pos_, float width_, float height_, Scene* scene_)
    : node{ node_ }, pos{ pos_ }, width{ width_ }, height{ height_ }, scene{ scene_ }, isPathTile{ false }, isDestination{ false }
{
}

void Tile::setPathTile(bool isPath) {
    isPathTile = isPath;
}

void Tile::setDestinationTile(bool isDest) {
    isDestination = isDest;
}

void Tile::render() const {
    glPushMatrix();

    glMultMatrixf(modelMatrix);  // Apply the tile’s model matrix

    if (isDestination) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green for destination tile
    }
    else if (isPathTile) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow for path tiles
    }
    else if (node && node->getIsBlocked()) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red for blocked tiles
    }
    else {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue for normal tiles
    }

    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(width, 0.0f, 0.0f);
    glVertex3f(width, height, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    glEnd();

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
