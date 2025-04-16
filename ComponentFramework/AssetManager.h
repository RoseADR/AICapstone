#pragma once
#include <string>
#include <iostream>
#include <unordered_map> 
#include "Component.h"
#include "Debug.h"

class AssetManager {
private:
	std::unordered_map<const char* , Ref<Component> > componentCatalog;
	static std::shared_ptr<AssetManager> instance;

public:
	
		
	AssetManager();
	~AssetManager();
	bool OnCreate();

	void RemoveAllComponents();
	void ListAllComponents() const;
	static std::shared_ptr<AssetManager> GetInstance();
	template<typename ComponentTemplate, typename ... Args>
	void AddComponent(const char* name, Args&& ... args_) {
		Ref<ComponentTemplate> t = std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...);
		componentCatalog[name] = t;
	}

	

	template<typename ComponentTemplate>
	Ref<ComponentTemplate> GetComponent(const char* name) const {
		auto id = componentCatalog.find(name);
		if (id == componentCatalog.end()) {
			Debug::Error("Can't fint requested component", __FILE__, __LINE__);
			return Ref<ComponentTemplate>(nullptr);
		}
		return std::dynamic_pointer_cast<ComponentTemplate>(id->second);
	}
	
};

