#ifndef SCENE3_H
#define SCENE3_H
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

class Scene3 : public Scene {
private:

	Ref<AssetManager> assetManager;
	Ref<CameraActor> camera;
	Ref<LightActor> light;
	Ref<Actor> gameboard;
	Ref<Actor> character;
	Ref<Actor> backGround;

	LocationManager locationManager;

	CollisionSystem collisionSystem;
	PhysicsSystem physicsSystem;

	Quaternion orientationBoard;
	Quaternion orientationCam;

	//FOR PATHFINDING
	class Graph* graph;
	std::vector<Node*> sceneNodes;
	float tileWidth, tileHeight;
	std::vector< std::vector<Tile*> > tiles;
	//Vec3 gridOffset;  // Offset to position the grid on the chessboard
	// Initializes tiles and calculates weights between nodes
	
	std::vector<std::vector<Tile*>> hackingTiles;
	bool showHackingGrid = false;
	//FOR DECISIONTREE
	DecisionTreeNode* decisionTreeRoot;
	void createHackingGrid();
	bool drawNormals;
	bool drawOverlay;
	bool isGrounded = false;
	bool isRunning;
	ISoundEngine* engine;

	float xAxis;
	float yAxis;
	Tile* currentlyHoveredTile = nullptr;


public:
	Vec2 hackingPlayerPos = Vec2(0, 0);
	bool showTiles = false;
	bool hackingMode = false;
	bool facing = true;
	explicit Scene3();
	virtual ~Scene3();
	std::vector<std::shared_ptr<Actor>> projectiles;

	virtual bool OnCreate();
	virtual void OnDestroy();

	void FireProjectile();

	virtual void Update(const float deltaTime);
	virtual void Render() const;
	virtual void HandleEvents(const SDL_Event& sdlEvent);

	int Pick(int x, int y);
	/// The w component holds the scale of the normal
	void DrawNormals(const Vec4 color) const;
	void DrawMeshOverlay(const Vec4 color) const;
	void LoadEnemies();

	
	/// The w component holds the scale of the normal
	


	//void FireProjectile(const Vec3& startPos, const Vec3& direction, float speed);

	// for sprite sheet animations
	bool facingRight;
	bool facingLeft;
	float currentTime;
	int index = 0;
	float frameSpeed = 0.1f;
};


#endif // SCENE3_H