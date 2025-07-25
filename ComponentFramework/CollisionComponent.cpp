#include "CollisionComponent.h"
#include <glew.h> 
#include "cmath"
#include "Actor.h"


CollisionComponent::CollisionComponent(Component* parent_, ColliderType type_, float radius_, Vec3 normal_, float dist_):
Component(parent_), colliderType(type_), radius(radius_), normal(normal_), dist(dist_) {

}


void CollisionComponent::SetAABB(const Vec3& position, const Quaternion& rotation, float rx_, float ry_, float rz_)
{
	// Original local extents (half-sizes)
	Vec3 extents = Vec3(rx_, ry_, rz_);

	// All 8 corners of the original AABB
	Vec3 corners[8] = {
		Vec3(-extents.x, -extents.y, -extents.z),
		Vec3(extents.x, -extents.y, -extents.z),
		Vec3(-extents.x,  extents.y, -extents.z),
		Vec3(extents.x,  extents.y, -extents.z),
		Vec3(-extents.x, -extents.y,  extents.z),
		Vec3(extents.x, -extents.y,  extents.z),
		Vec3(-extents.x,  extents.y,  extents.z),
		Vec3(extents.x,  extents.y,  extents.z)
	};

	// Initialize rotated extents
	Vec3 minCorner(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 maxCorner(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// Rotate all corners and find the new min/max extents
	for (int i = 0; i < 8; ++i) {
		Vec3 rotatedCorner = rotation * corners[i];  // assuming operator* overloaded for Quaternion * Vec3

		minCorner.x = std::min(minCorner.x, rotatedCorner.x);
		minCorner.y = std::min(minCorner.y, rotatedCorner.y);
		minCorner.z = std::min(minCorner.z, rotatedCorner.z);

		maxCorner.x = std::max(maxCorner.x, rotatedCorner.x);
		maxCorner.y = std::max(maxCorner.y, rotatedCorner.y);
		maxCorner.z = std::max(maxCorner.z, rotatedCorner.z);
	}

	// Compute new AABB center and extents
	Vec3 rotatedCenter = (maxCorner + minCorner) * 0.5f;
	Vec3 rotatedExtents = (maxCorner - minCorner) * 0.5f;

	// Translate to world position
	Vec3 finalCenter = position + rotatedCenter;

	// Assign final AABB
	aabb = { finalCenter, rotatedExtents.x, rotatedExtents.y, rotatedExtents.z };
}



void CollisionComponent::DrawAABB() const
{
	if (colliderType != ColliderType::AABB) return;

	const Vec3& c = aabb.center;
	float rx = aabb.rx, ry = aabb.ry, rz = aabb.rz;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.0f, 0.0f); // bright red

	glBegin(GL_QUADS);

	// Front face
	glVertex3f(c.x - rx, c.y - ry, c.z + rz);
	glVertex3f(c.x + rx, c.y - ry, c.z + rz);
	glVertex3f(c.x + rx, c.y + ry, c.z + rz);
	glVertex3f(c.x - rx, c.y + ry, c.z + rz);

	// Back face
	glVertex3f(c.x - rx, c.y - ry, c.z - rz);
	glVertex3f(c.x + rx, c.y - ry, c.z - rz);
	glVertex3f(c.x + rx, c.y + ry, c.z - rz);
	glVertex3f(c.x - rx, c.y + ry, c.z - rz);

	// Sides
	glVertex3f(c.x - rx, c.y - ry, c.z - rz);
	glVertex3f(c.x - rx, c.y - ry, c.z + rz);
	glVertex3f(c.x - rx, c.y + ry, c.z + rz);
	glVertex3f(c.x - rx, c.y + ry, c.z - rz);

	glVertex3f(c.x + rx, c.y - ry, c.z - rz);
	glVertex3f(c.x + rx, c.y - ry, c.z + rz);
	glVertex3f(c.x + rx, c.y + ry, c.z + rz);
	glVertex3f(c.x + rx, c.y + ry, c.z - rz);

	glVertex3f(c.x - rx, c.y - ry, c.z - rz);
	glVertex3f(c.x + rx, c.y - ry, c.z - rz);
	glVertex3f(c.x + rx, c.y - ry, c.z + rz);
	glVertex3f(c.x - rx, c.y - ry, c.z + rz);

	glVertex3f(c.x - rx, c.y + ry, c.z - rz);
	glVertex3f(c.x + rx, c.y + ry, c.z - rz);
	glVertex3f(c.x + rx, c.y + ry, c.z + rz);
	glVertex3f(c.x - rx, c.y + ry, c.z + rz);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
