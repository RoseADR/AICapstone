#ifndef SCENE4_H
#define SCENE4_H
#include "Scene.h"
#include "Vector.h"
#include "Matrix.h"
#include "Component.h"
#include "AssetManager.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class CameraActor;
class LightActor;
class Actor;

class Scene4 : public Scene {
private:
	Ref<AssetManager> assetManager;
	Ref<CameraActor> camera;
	Ref<LightActor> light;
	Ref<Actor> gameboard;
	Ref<Actor> backGround;
	bool drawNormals;
	bool drawOverlay;
public:
	explicit Scene4();
	virtual ~Scene4();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime);
	virtual void Render() const;
	virtual void HandleEvents(const SDL_Event& sdlEvent);

	int Pick(int x, int y);
	/// The w component holds the scale of the normal
	void DrawNormals(const Vec4 color) const;

	void DrawMeshOverlay(const Vec4 color) const;

};


#endif // SCENE2_H