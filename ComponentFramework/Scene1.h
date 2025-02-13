#ifndef SCENE1_H
#define SCENE1_H
#include "Scene.h"
#include "Vector.h"
#include "Matrix.h"
#include "Component.h"
#include "AssetManager.h"
#include "LocationManager.h"
#include "Graph.h"
#include "Node.h"
#include "Tile.h"
#include "Action.h"
#include "DecisionTreeNode.h"
#include "TreeBuilder.h"
#include "ScottCollisionSystem.h"
#include "PhysicsSystem.h"
#include <irrKlang.h>

using namespace MATH;
using namespace irrklang;
/// let me change
/// Forward declarations 
union SDL_Event;
class CameraActor;
class LightActor;
class Actor;

class Scene1 : public Scene {
private:

	Ref<AssetManager> assetManager;
	Ref<CameraActor> camera;
	Ref<LightActor> light;
	Ref<Actor> gameboard;
	Ref<Actor> factory;
	Ref<Actor> bg;
	Ref<Actor> Bridge;
	Ref<Actor> house;
	Ref<Actor> bill;
	Ref<Actor> character;
	Ref<Actor> TestCube;
	Ref<Actor> enemies;
	Ref<Actor> projectile;
	

	LocationManager locationManager;

	CollisionSystem collisionSystem;
	PhysicsSystem physicsSystem;
	
	Quaternion orientationBoard;
	Quaternion orientationBg;
	Quaternion orientationCam;
	Quaternion orientationHouse;
	Quaternion orientationBill;//needed for the orientation of the tiles to match the board (evetually ground)

	//FOR PATHFINDING
	class Graph* graph;
	std::vector<Node*> sceneNodes;
	float tileWidth, tileHeight;
	std::vector< std::vector<Tile*> > tiles;
	//Vec3 gridOffset;  // Offset to position the grid on the chessboard
	// Initializes tiles and calculates weights between nodes
	void createTiles();
	void calculateConnectionWeights();

	//FOR DECISIONTREE
	DecisionTreeNode* decisionTreeRoot;

	bool drawNormals;
	bool drawOverlay;
	bool isGrounded = false;
	bool isRunning;
	ISoundEngine* engine;

	float xAxis;
	float yAxis;

	

public:
	bool showTiles = false;
	bool facing = true;
	explicit Scene1();
	virtual ~Scene1();
	std::vector<std::shared_ptr<Actor>> projectiles;

	virtual bool OnCreate();
	virtual void OnDestroy();

	void FireProjectile();

	virtual void Update(const float deltaTime);
	virtual void Render() const;
	virtual void HandleEvents(const SDL_Event &sdlEvent);

	int Pick(int x, int y);
	/// The w component holds the scale of the normal
	void DrawNormals(const Vec4 color) const;
	void DrawMeshOverlay(const Vec4 color) const;
	void LoadEnemies();

	//void FireProjectile(const Vec3& startPos, const Vec3& direction, float speed);

	// for sprite sheet animations
	bool facingRight;
	bool facingLeft;
	float currentTime;
	int index = 0;
	float frameSpeed = 0.1f;
};


#endif // SCENE1_H