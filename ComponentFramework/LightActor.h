#pragma once
#include <glew.h>
#include "Actor.h"
#include "Vector.h"

using namespace MATH;

// LightStyle enum defines different types of lights that can be used in the game
enum class LightStyle {
	DirectionLight = 1, // Directional light, like the sun, affects all objects in the same direction
	PointLight,			// Point light, emits light from a single point in all directions
	SkyLight,			// Light coming from the sky or environment
	SpotLight			// Spotlight, focused light in a specific direction
};

// LightActor class represents a light source in the scene. It inherits from Actor class
class LightActor : public Actor {
	// Prevents copying or moving a LightActor object, ensuring it is only created once
	LightActor(const LightActor&) = delete;
	LightActor(LightActor&&) = delete;
	LightActor& operator=(const LightActor&) = delete;
	LightActor& operator=(LightActor&&) = delete;
private:
	LightStyle lightStyle;	// The type of light (directional, point, etc.)
	Vec3 position;			// Position of the light in 3D space
	Vec4 colour;			// Colour of the light (RGBA)
	float intensity;		// Brightness of the light
	Vec3 fallOff;			// How the light intensity decreases over distance
	GLuint shadowBufferID;	// ID of the shadow buffer, used for shadow mapping
	GLuint depthTextureID;	// ID of the depth texture, used for depth mapping

	GLuint uboLightDataID;	// ID for Uniform Buffer Object holding light data
	const GLuint bindPoint = 1; // The bind point for the Uniform Buffer Object
public:
	// Constructor to initialize a LightActor with specified properties
	LightActor(Component* parent_, LightStyle lightStyle_, Vec3 location_, Vec4  colour_,
		float intensity = 1.0f, Vec3 fallOff = Vec3(0.0f, 0.0f, 0.0f));
	// Destructor to clean up resources when the light actor is destroyed
	~LightActor();
	// Getter function to return the position of the light
	Vec3 GetPosition() const { return position; }
	// Setter function to update the position of the light and ensure UBO is updated
	void SetPosition(const Vec3 position_ ) { 
		position = position_; 
		UpdateUBO();	// Update the UBO with the new light position
	}
	// Function to update the Uniform Buffer Object with the latest light data
	void UpdateUBO() const;
	// Called when the actor is created, used for initializing light-related resources
	bool OnCreate() override;
	// Called when the actor is destroyed, used for cleaning up light-related resources
	void OnDestroy() override;
	// Function to render the light (though usually, lights themselves are not rendered)
	void Render() const;
	// Getter function to return the light's UBO ID
	GLuint GetLightID() { return uboLightDataID; }

	/// just for debugging
	// For debugging purposes: matrices used for depth projections and views (for shadow mapping)
	Matrix4 depthProjectionMatrix;
	Matrix4 depthViewMatrix;

};

