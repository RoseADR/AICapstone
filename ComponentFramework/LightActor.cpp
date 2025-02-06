#include "LightActor.h"
#include "CameraActor.h"
#include "UBO_Padding.h"
#include "MMath.h"

using namespace MATH;

// Constructor for the LightActor class
LightActor::LightActor(Component* parent_, LightStyle lightStyle_, Vec3 location_, Vec4  colour_,
	float intensity, Vec3 fallOff ): Actor(parent_) {
	// Initialize light properties (style, position, color, etc.)
	lightStyle = lightStyle_;
	position = location_;
	colour = colour_;
	

}

// This method updates the UBO (Uniform Buffer Object) with the light's data
void LightActor::UpdateUBO() const {
	Matrix4 viewMatrix; /// creates default identity matrix
	
	// Bind the UBO (Uniform Buffer Object) to store the light data
	glBindBuffer(GL_UNIFORM_BUFFER, uboLightDataID);
	// Calculate the size of the data to be added to the buffer (using padding values)
	size_t buffer_size = UBO_PADDING::VEC3 + UBO_PADDING::VEC4;
	size_t offset = 0;
	// Update the position data in the buffer
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Vec3),position);
	offset = UBO_PADDING::VEC3;// Move the offset for the next data

	// Update the color data in the buffer
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(Vec4), colour);
	
}

// This method sets up the necessary resources for the light actor
bool LightActor::OnCreate() {
	// If the object is already created, just return true
	if (isCreated) return isCreated;

	// Create the depth projection matrix (for shadow mapping)
	depthProjectionMatrix = MMath::orthographic(-7, 7, -10, 10, -10, 20);
	// Create the view matrix for the light actor (where the light is positioned)
	depthViewMatrix = MMath::lookAt(-position, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));

	// Define the size of the buffer (taking padding into account)
	size_t buffer_size = UBO_PADDING::VEC3 + UBO_PADDING::VEC4;

	// Generate and bind the UBO for the light data
	glGenBuffers(1, &uboLightDataID);
	glBindBuffer(GL_UNIFORM_BUFFER, uboLightDataID);
	glBufferData(GL_UNIFORM_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
	// Update the UBO with the light's data (position and color)
	UpdateUBO();
	// Unbind the UBO
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	/// To connect this ubo to the bindingpoint, the buffer does not need to be see the above line
	// Bind the UBO to the OpenGL uniform buffer binding point(connection with shaders)
	glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, uboLightDataID);


	// Generate and set up the framebuffer for shadow rendering
	glGenFramebuffers(1, &shadowBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBufferID);
	
	// Generate and set up the depth texture for the shadow map
	glGenTextures(1, &depthTextureID);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach the depth texture to the framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureID, 0);
	// Disable color output to the framebuffer (we only need depth information)
	glDrawBuffer(GL_NONE); 
	// Unbind the framebuffer (done setting it up)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Mark the object as created
	isCreated = true;
	return isCreated;
}

// Destructor, cleans up resources when the object is destroyed
LightActor::~LightActor() {
	OnDestroy();
}

// This method renders the light actor
void LightActor::Render() const {


}

// This method destroys the resources used by the light actor
void LightActor::OnDestroy() {
	// Delete the UBO, texture, and framebuffer when the object is destroyed
	glDeleteBuffers(1, &uboLightDataID);
	glDeleteTextures(1, &depthTextureID);
	glDeleteFramebuffers(1, &shadowBufferID);
	// Mark the object as not created anymore
	isCreated = false;
}
