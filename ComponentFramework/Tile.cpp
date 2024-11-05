#include "Tile.h"
#include <glew.h>

Tile::Tile(Node* node_, Vec3 pos_, float width_, float height_, Scene* scene_, Quaternion orientation_)
    : node{ node_ }, pos{ pos_ }, width{ width_ }, height{ height_ }, scene{ scene_ }, orientation{ orientation_ }
{
}


void Tile::render() const
{
    glPushMatrix();

    // Position the tile in the grid
    glTranslatef(pos.x, pos.y, pos.z);

    // Apply rotation to match the chessboard orientation
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color for the tile
    glVertex3f(0.0f, 0.0f, 0.0f);            // Bottom-left corner
    glVertex3f(width, 0.0f, 0.0f);           // Bottom-right corner
    glVertex3f(width, height, 0.0f);         // Top-right corner
    glVertex3f(0.0f, height, 0.0f);          // Top-left corner
    glEnd();

    glPopMatrix();

}


