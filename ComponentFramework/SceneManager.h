#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>

#include "irrKlang.h"
using namespace irrklang;

class SceneManager  {
public:
	
	SceneManager();
	~SceneManager();
	void Run();
	bool Initialize(std::string name_, int width_, int height_);
	void HandleEvents();
	float playerHealth = 100.0f; // Current health
	float maxHealth = 100.0f;    
	int totalAmmo = 15;  
	int clipAmmo = 15;     // Bullets currently in clip
	int clipSize = totalAmmo;     
	static SceneManager* GetInstance();
	bool dead = false;
	bool triggerScene1 = false;
	bool Victory = false;
	ISoundEngine* engine;

private:
	enum class SCENE_NUMBER {
		SCENE0 = 0,
		SCENE1,
		SCENE2,
		SCENE3,
		SCENE4,
		SCENE5,
		SCENE6
	};

	class Scene* currentScene;
	class Timer* timer;
	class Window* window;

	static SceneManager* instance;

	bool show_demo_window;
	bool show_another_window;


	unsigned int fps;
	bool isRunning;
	bool fullScreen;
	void BuildNewScene(SCENE_NUMBER scene_);
};


#endif // SCENEMANAGER_H