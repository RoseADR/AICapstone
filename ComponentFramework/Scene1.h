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
#include "CollisionSystem.h"
#include "PhysicsSystem.h"
#include "TransformSystem.h"
#include <irrKlang.h>
#include "SceneManager.h"
#include <unordered_map>
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

	SceneManager* sceneManager;

	Ref<AssetManager> assetManager;
	Ref<CameraActor> camera;
	Ref<LightActor> light;
	Ref<Actor> deathFloor;
	Ref<Actor> factory;
	Ref<Actor> stairs;
	Ref<Actor> topFloor;
	Ref<Actor> Bridge2;
	Ref<Actor> bg;
	Ref<Actor> projectile;
	Ref<Actor> Bridge;
	Ref<Actor> Barrel;
	Ref<Actor> house;
	Ref<Actor> hack;
	Ref<Actor> bill;
	Ref<Actor> character;
	Ref<Actor> TestCube;
	Ref<Actor> TestCube1;
	Ref<Actor> TestCube2;
	Ref<Actor> enemies;
	//Ref<Actor> tile;
	Ref<Actor> aabbBox;
	Ref<Actor> factoryCollisionBox;
	Ref<Actor> BridgeCollisionBox;
	

	LocationManager locationManager;

	CollisionSystem collisionSystem;
	PhysicsSystem physicsSystem;
	TransformSystem transformSystem;
	
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
	void SpawnAmmoAt(const Vec3& position);
	std::vector<Ref<Actor>> ammoPickups;


	void calculateConnectionWeights();
	std::vector<std::vector<Tile*>> hackingTiles;
	bool showHackingGrid = false;
	//FOR DECISIONTREE
	DecisionTreeNode* decisionTreeRoot;
	void createHackingGrid();
	bool drawNormals;
	bool drawOverlay;
	bool isGrounded = false;
	bool isRunning;
	bool hackUsed = false;
	bool isJumping = false;
	float jumpVelocity = 7.5f;
	float footstepTimer = 0.0f;
	float footstepCooldown = 0.6f;
	//Audio
	ISoundEngine* engine;
	//
	Ref<Actor> loadingScreenActor;
	bool loadingStarted = false;
	float loadingTimer = 0.0f;
	void SpawnLoadingScreen();


	float xAxis;
	float yAxis;
	Tile* currentlyHoveredTile = nullptr;
	std::vector<std::pair<int, int>> redTilePositions;
	
public:
	std::unordered_map<Actor*, DecisionTreeNode*> enemyDecisionTrees;

	std::unordered_map<Actor*, float> enemyHealth;
	Vec2 hackingPlayerPos = Vec2(0, 0);
	bool showTiles = false;
	bool hackingMode = false;
	bool facing = true;
	explicit Scene1(SceneManager* manager);
	virtual ~Scene1();
	std::vector<std::shared_ptr<Actor>> projectiles;

	virtual bool OnCreate();
	virtual void OnDestroy();

	void FireProjectile();
	void Reload();
	virtual void Update(const float deltaTime);
	virtual void Render() const;
	virtual void HandleEvents(const SDL_Event &sdlEvent);

	

	int Pick(int x, int y);
	/// The w component holds the scale of the normal
	void DrawNormals(const Vec4 color) const;
	void DrawMeshOverlay(const Vec4 color) const;
	void LoadEnemies(const Vec3& position);
	
	//void FireProjectile(const Vec3& startPos, const Vec3& direction, float speed);

	// for sprite sheet animations
	bool facingRight;
	bool facingLeft;
	bool movingUp;
	bool movingDown;
	bool idleTexture;
	float currentTime;
	Vec2 index;
	float frameSpeed = 0.01f;

	uint64_t frameCount = 0;

	void AddAABBCollisionBox(Ref<Actor> actor, const Vec3& pos, const Quaternion& rot, const Vec3& scale);

};


#endif // SCENE1_H