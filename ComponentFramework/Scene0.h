#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include "Matrix.h"
#include "Component.h"
#include "AssetManager.h"
#include <chrono>
#include <thread>

#include "irrKlang.h"


using namespace MATH;
using namespace irrklang;


/// Forward declarations 
union SDL_Event;
class CameraActor;
class LightActor;
class Actor;

class Scene0 : public Scene {
private:
	Ref<AssetManager> assetManager;
	Ref<CameraActor> camera;
	Ref<LightActor> light;
	Ref<Actor> backGround;
	bool drawNormals;
	bool drawOverlay;
	bool isRunning;
	

	ISoundEngine* engine;

public:
	explicit Scene0();
	virtual ~Scene0();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime);
	virtual void Render() const;
	virtual void HandleEvents(const SDL_Event &sdlEvent);

	void DrawNormals(const Vec4 color) const;
	void DrawMeshOverlay(const Vec4 color) const;
	int Pick(int x, int y);
};


#endif // SCENE0_H