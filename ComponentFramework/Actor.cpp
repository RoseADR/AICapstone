#include "Actor.h"
#include "Debug.h"
#include <glew.h>
#include "ShaderComponent.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"

// Constructor for the Actor class, which initializes the parent component
Actor::Actor(Component* parent_):Component(parent_) {}

// OnCreate is called to initialize the Actor and its components
// It returns 'true' if the Actor is successfully created, 'false' otherwise
bool Actor::OnCreate() {
	// If the Actor has already been created, just return 'true' to avoid re-initializing
	if (isCreated) return true;
	// Log that we are loading assets for the Actor
	Debug::Info("Loading assets for Actor: ", __FILE__, __LINE__);
	// Iterate through all components of the Actor and initialize each one
	for (auto component : components) {
		// If a component fails to initialize, log an error and return 'false'
		if (component->OnCreate() == false) {
			Debug::Error("Loading assets for Actor/Components: ", __FILE__, __LINE__);
			isCreated = false;
			return isCreated;
		}
	}
	// If all components are successfully initialized, set 'isCreated' to 'true'
	isCreated = true;
	return isCreated;
}

// Destructor for the Actor class, which is responsible for cleaning up resources when the Actor is destroyed
Actor::~Actor() {
	OnDestroy(); // Call OnDestroy to clean up components
}

// OnDestroy cleans up the Actor and its components, called during destruction
void Actor::OnDestroy() {
	// Log we are deleting assets for the Actor
	Debug::Info("Deleting assets for Actor: ", __FILE__, __LINE__);
	// Remove all components from the Actor
	RemoveAllComponents();
	// Set 'isCreated' to false, indicating the Actor is no longer created
	isCreated = false;
}


// Update is called every frame to update the Actor's state. 
void Actor::Update(const float deltaTime) {
	//std::cout << "Hello from Update\n";
}

// Render is called to draw the Actor to the screen. 
void Actor::Render()  {
	auto mesh = GetComponent<MeshComponent>();
	auto shader = GetComponent<ShaderComponent>();
	auto material = GetComponent<MaterialComponent>();

	// Bind shader only if it's valid
	if (shader) {
		GLuint programID = shader->GetProgram();
		if (programID != 0) {
			glUseProgram(programID);

			// If material exists, bind texture
			if (material) {
				GLuint texID = material->getTextureID();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texID);

				GLuint texUniformID = shader->GetUniformID("textureSampler");
				if (texUniformID != static_cast<GLuint>(-1)) {
					glUniform1i(texUniformID, 0);
				}
			}

			// Apply model matrix if shader accepts it
			GLuint modelLoc = shader->GetUniformID("modelMatrix");
			if (modelLoc != static_cast<GLuint>(-1)) {
				Matrix4 modelMatrix = GetModelMatrix();
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, reinterpret_cast<const float*>(&modelMatrix));
			}
		}
	}

	// Render mesh even without shader (if some legacy fixed-function fallback)
	if (mesh) {
		mesh->Render();
	}

	// Unbind shader program
	if (shader && shader->GetProgram() != 0) {
		glUseProgram(0);
	}
}


// RemoveAllComponents removes all components from the Actor. 
void Actor::RemoveAllComponents() {
	 
}

// ListComponents prints out the names of all the components attached to this Actor
void Actor::ListComponents() const {
	// Print the name of the Actor's class
	std::cout << typeid(*this).name() << " contains the following components:\n";
	// Iterate through each component and print its type name
	for (Ref<Component> component : components) {
		std::cout << typeid(*component).name() << std::endl;
	}
	std::cout << '\n';
}

// GetModelMatrix returns the model matrix of the Actor, used for transformations like position, rotation, and scale
Matrix4 Actor::GetModelMatrix() {
	// Try to get the TransformComponent, which contains the transformation data for this Actor
	Ref<TransformComponent> transform = GetComponent<TransformComponent>();
	// If the Actor has a TransformComponent, retrieve its transformation matrix
	if (transform.get()) {
		modelMatrix = transform->GetTransformMatrix();
	} else {
		// If no TransformComponent exists, reset the model matrix to the identity matrix (no transformation)
		modelMatrix.loadIdentity();
	}
	// If this Actor has a parent (another Actor), combine the parent's model matrix with this Actor's
	if (parent) {

		modelMatrix = dynamic_cast<Actor*>(parent)->GetModelMatrix() * modelMatrix;
	}
	return modelMatrix; // Return the final model matrix
} 