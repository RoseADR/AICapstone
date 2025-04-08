#include <memory>
#include <iostream>
#include <fstream>
#include "tinyxml2.h"
#include <string>
#include "AssetManager.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "MaterialComponent.h"
#include "AiComponent.h"


AssetManager::AssetManager() {
	
	AddComponent<MeshComponent>("Sphere", nullptr, "meshes/Sphere.obj");
	AddComponent<MeshComponent>("Mario", nullptr, "meshes/Mario.obj");
	AddComponent<MeshComponent>("Cube", nullptr, "meshes/Cube.obj");
	AddComponent<MeshComponent>("Plane", nullptr, "meshes/Plane.obj");
	AddComponent<MeshComponent>("Square", nullptr, "meshes/Square.obj");
	AddComponent<MeshComponent>("House", nullptr, "meshes/cartoonHouse.obj");
	AddComponent<MeshComponent>("Board", nullptr, "meshes/billboard.obj");
	AddComponent<MeshComponent>("Bridge", nullptr, "meshes/bridge.obj");
	AddComponent<MeshComponent>("StreetLight", nullptr, "meshes/streetLight.obj");
	AddComponent<MeshComponent>("Factory", nullptr, "meshes/Factory.obj");
	AddComponent<MeshComponent>("Car", nullptr, "meshes/Car.obj");
	AddComponent<MeshComponent>("Scaffolding", nullptr, "meshes/Scaffolding.obj");
	AddComponent<MeshComponent>("Blocker", nullptr, "meshes/blocker.obj");
	//AddComponent<MeshComponent>("Robot", nullptr, "meshes/robot.obj");
	AddComponent<MeshComponent>("Bridge2", nullptr, "meshes/bridge2.obj");
	AddComponent<MeshComponent>("Ammo", nullptr, "meshes/AmmoBox.obj");
	AddComponent<MeshComponent>("Bus", nullptr, "meshes/Bus.obj");




	AddComponent<MeshComponent>("Tree", nullptr, "meshes/tallTree.obj");
	AddComponent<MeshComponent>("Tree2", nullptr, "meshes/roundTree.obj");
	AddComponent<MeshComponent>("Tree3", nullptr, "meshes/tallTree2.obj");
	AddComponent<MeshComponent>("Tree4", nullptr, "meshes/roundTree2.obj");



	AddComponent<MeshComponent>("Box", nullptr, "meshes/box_wooden.obj");
	AddComponent<MeshComponent>("Barrel", nullptr, "meshes/barrel.obj");
	AddComponent<MeshComponent>("Tunnels", nullptr, "meshes/Tunnels.obj");
	AddComponent<MeshComponent>("UTunnel", nullptr, "meshes/UTunnel.obj");
	AddComponent<MeshComponent>("StreetWall", nullptr, "meshes/streetWalls.obj");

	AddComponent<MeshComponent>("Fences", nullptr, "meshes/Fences.obj");

	
	AddComponent<ShaderComponent>("DefaultShader",nullptr,"shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	AddComponent<ShaderComponent>("ColorPickingShader",nullptr,"shaders/colorPickingVert.glsl", "shaders/colorPickingFrag.glsl");
	AddComponent<ShaderComponent>("TextureShader",nullptr,"shaders/textureVert.glsl", "shaders/textureFrag.glsl");
	AddComponent<ShaderComponent>("ShadowDepthShader",nullptr,"shaders/shadowDepthVert.glsl", "shaders/shadowDepthFrag.glsl");
	AddComponent<ShaderComponent>("DrawNormalsShader",nullptr,"shaders/drawNormalsVert.glsl", "shaders/drawNormalsFrag.glsl","shaders/drawNormalsGeom.glsl");
	AddComponent<ShaderComponent>("WireFrameShader",nullptr,"shaders/wireFrameVert.glsl", "shaders/wireFrameFrag.glsl","shaders/wireFrameGeom.glsl");
	AddComponent<ShaderComponent>("Billboard", nullptr, "shaders/BillBoardingVert.glsl", "shaders/BillBoardingFrag.glsl");
	
	
	AddComponent<MaterialComponent>("WalkSpriteSheet", nullptr, "textures/robot.png");
	AddComponent<MaterialComponent>("houseText", nullptr, "textures/cartoonHouseDef.png");
	
	AddComponent<MaterialComponent>("road", nullptr, "textures/road1.jpg");
	AddComponent<MaterialComponent>("billAds", nullptr, "textures/billBoardAds.png");
	AddComponent<MaterialComponent>("bg", nullptr, "textures/landscape.png");
	AddComponent<MaterialComponent>("city", nullptr, "textures/city3.png");
	AddComponent<MaterialComponent>("WoodBox", nullptr, "textures/box_wooden_color.png");
	AddComponent<MaterialComponent>("BarrelDif", nullptr, "textures/barrel_skin.png");
	AddComponent<MaterialComponent>("BulletSkin", nullptr, "textures/Bullet.png");
	AddComponent<MaterialComponent>("GiantRobot", nullptr, "textures/GiantRobotSpriteSheet.png");
	AddComponent<MaterialComponent>("stone", nullptr, "textures/stone.jpg");
	AddComponent<MaterialComponent>("CarText", nullptr, "textures/carText.png");
	AddComponent<MaterialComponent>("BlockerText", nullptr, "textures/blockerText.png");
	AddComponent<MaterialComponent>("lavaTexture", nullptr, "textures/lava.png");
	AddComponent<MaterialComponent>("RoboGun", nullptr, "textures/FixedRobotJumpGun.png");
	AddComponent<MaterialComponent>("RoboMainMenu", nullptr, "textures/MainMenuRobot.png");
	AddComponent<MaterialComponent>("Enemy", nullptr, "textures/RobotEnemy.png");
	AddComponent<MaterialComponent>("GameOver", nullptr, "textures/GameOver.png");
	AddComponent<MaterialComponent>("AmmoDif", nullptr, "textures/AmmoDif.png");
	AddComponent<MaterialComponent>("grass", nullptr, "textures/grass.jpg");
	AddComponent<MaterialComponent>("tallTree", nullptr, "textures/treeTall.png");
	AddComponent<MaterialComponent>("roundTree", nullptr, "textures/treeRound.png");
	AddComponent<MaterialComponent>("BusDif", nullptr, "textures/busDif.png");

	//AddComponent<MaterialComponent>("RobotTexture", nullptr, "textures/robo.png");

	OnCreate();
	//ListAllComponents();
}

AssetManager::~AssetManager() {
	RemoveAllComponents();
}

bool AssetManager::OnCreate() {
	/// This is why auto cool sometimes
	for (std::pair<const char*, Ref<Component>> c : componentCatalog) {
		if (c.second->OnCreate()  == false) {
			/// Report error
			return false;
		}
	}
	return true;
}


void AssetManager::RemoveAllComponents() {
	componentCatalog.clear();
}

void AssetManager::ListAllComponents() const {
	for (auto c : componentCatalog) {
		std::cout << c.first << ": " << typeid(*(c.second.get()) ).name() << std::endl;
	}
}

