#pragma once
#include <vector> 
#include <iostream>
#include "Component.h"
#include "TransformComponent.h" 
#include "PhysicsComponent.h"

// Actor class inherits from Component, representing an entity in the game with attached components
class Actor: public Component {
	/// Unless you know what these do don't allow them to be created implicitly 
	// Delete copy and move constructors and assignment operators to prevent copying or moving of an Actor
	Actor(const Actor&) = delete; // Stops copy construction
	Actor(Actor&&) = delete; // Stops move construction
	Actor& operator=(const Actor&) = delete; // Avoids copy assignments
	Actor& operator=(Actor&&) = delete; // Avoids move assignments

private:
	std::vector< Ref<Component> > components; // creates a list of components attached to this Actor
	Matrix4 modelMatrix; // Transformation matrix to represent the Actor's position and rotation
	std::string name; // for giving names
public:

	//setting name
	void SetName(const std::string& n) { name = n; }
	std::string GetName() const { return name; }


	// Constructor: Initializes an Actor with a parent (usually another Actor or component)
	Actor(Component* parent_);
	// Destructor: Cleans up the Actor when it is destroyed
	~Actor();
	// Called to initialize the Actor and its components
	bool OnCreate();
	// Called to clean up the Actor and its components
	void OnDestroy();
	// Update method to update the Actor each frame
	void Update(const float deltaTime_);
	// Render method to render the Actor
	void Render() const;

	//ADDED FOR DECISION TREE MAY NEED TO PUT SOMEWHERE ELSE
	int health = 100;


	// Templated method to add a component to the Actor
	// This version of AddComponent ensures that the same type of component is not added twice
	template<typename ComponentTemplate>
	void AddComponent(Ref<ComponentTemplate> component_) {
		// Check if the component type is already added to the Actor
		if (GetComponent<ComponentTemplate>().get() != nullptr) {
#ifdef _DEBUG
			std::cerr << "WARNING: Trying to add a component type that is already added - ignored\n";
#endif
			return; // If component already exists, exit the function
		}
		components.push_back(component_);// Add the component to the components list
	}
	
	// Templated method to create and add a component to the Actor using variadic arguments
	template<typename ComponentTemplate, typename ... Args>
	void AddComponent(Args&& ... args_) {
		/// before you add the component ask if you have the component in the list already,
		/// if so - don't add a second one. 
		if (GetComponent<ComponentTemplate>().get() != nullptr) {
#ifdef _DEBUG
			std::cerr << "WARNING: Trying to add a component type that is already added - ignored\n";
#endif
			return;
		}
		/// Finish building the component and add the component to the list
		components.push_back(std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...));
	}

	// Template method to retrieve a specific component attached to the Actor
	template<typename ComponentTemplate>
	Ref<ComponentTemplate> GetComponent() const {
		// Iterate through the components to find the one of the requested type.
		for (auto component : components) {
			// Use dynamic cast to check if the component is of the required type
			if (dynamic_cast<ComponentTemplate*>(component.get())) {
				/// This is a dynamic cast designed for shared_ptr's
				/// https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
				return std::dynamic_pointer_cast<ComponentTemplate>(component);
			}
		}
		// Return a null shared pointer if the component is not found
		return Ref<ComponentTemplate>(nullptr);
	}

	// Template method to remove a specific component by type
	template<typename ComponentTemplate>
	void RemoveComponent() {
		// Iterate through the components list to find the component to remove
		for (unsigned int i = 0; i < components.size(); i++) {
			// If component of the specified type is found, remove it from the list
			if (dynamic_cast<ComponentTemplate*>(components[i].get()) != nullptr) {
				components.erase(components.begin() + i); // Erase component from the vector
				break; // Exit after removing the component
			}
		}
	}

	// Method to remove all components from the Actor
	void RemoveAllComponents();
	// Method to print out a list of all components attached to the Actor
	void ListComponents() const;
	// Method to retrieve the Actor's model transformation matrix
	Matrix4 GetModelMatrix();

};

