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

using namespace MATH;
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
	Ref<Actor> character;
	Ref<Actor> enemies;
	LocationManager locationManager;
	
	Quaternion orientationBoard; //needed for the orientation of the tiles to match the board (evetually ground)
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
public:
	bool showTiles = false;
	explicit Scene1();
	virtual ~Scene1();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime);
	virtual void Render() const;
	virtual void HandleEvents(const SDL_Event &sdlEvent);

	int Pick(int x, int y);
	/// The w component holds the scale of the normal
	void DrawNormals(const Vec4 color) const;

	void DrawMeshOverlay(const Vec4 color) const;
	
	void LoadEnemies();
};


#endif // SCENE1_H