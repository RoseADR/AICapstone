#include "Tile.h"
#include <glew.h>

Tile::Tile(Node* node_, Vec3 pos_, float width_, float height_, Scene* scene_)
    : node{ node_ }, pos{ pos_ }, width{ width_ }, height{ height_ }, scene{ scene_ } 
{
}


void Tile::render() const
{

    glPushMatrix();

    // Apply grid-wide rotation to align with the chessboard MOVING TO SCENE RENDER PER MEETING WITH GAIl
    //glRotatef(0.0f, 1.0f, 1.0f, 1.0f);  // Rotate around x-axis

    // Position the tile in the grid
    glTranslatef(pos.x, pos.y, pos.z);

    // Set color based on whether the tile's node is blocked
    if (node && node->getIsBlocked()) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red color for blocked tiles
    }
    else {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue color for normal tiles
    }

    // Render the tile as a filled quad
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);            // Bottom-left corner
    glVertex3f(width, 0.0f, 0.0f);           // Bottom-right corner
    glVertex3f(width, height, 0.0f);         // Top-right corner
    glVertex3f(0.0f, height, 0.0f);          // Top-left corner
    glEnd();

    // Draw the border around the tile without depth test
    glDisable(GL_DEPTH_TEST);  // Temporarily disable depth testing for the border
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the outline
    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(width, 0.0f, 0.0f);
    glVertex3f(width, height, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    glEnd();
    glEnable(GL_DEPTH_TEST);  // Re-enable depth testing

    glPopMatrix();

}


