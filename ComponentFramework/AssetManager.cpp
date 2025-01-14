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

	

	AddComponent<MeshComponent>("Plane", nullptr,"meshes/Plane8x8.obj");

	AddComponent<MeshComponent>("CheckerPiece", nullptr,"meshes/CheckerPiece.obj");
	AddComponent<MeshComponent>("Pawn", nullptr,"meshes/Pawn.obj");
	AddComponent<MeshComponent>("Rook", nullptr,"meshes/Rook.obj");
	AddComponent<MeshComponent>("Bishop", nullptr,"meshes/Bishop.obj");
	AddComponent<MeshComponent>("Knight", nullptr,"meshes/Knight.obj");
	AddComponent<MeshComponent>("Queen", nullptr, "meshes/Queen.obj");
	AddComponent<MeshComponent>("King", nullptr, "meshes/King.obj");
	
	AddComponent<ShaderComponent>("DefaultShader",nullptr,"shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	AddComponent<ShaderComponent>("ColorPickingShader",nullptr,"shaders/colorPickingVert.glsl", "shaders/colorPickingFrag.glsl");
	AddComponent<ShaderComponent>("TextureShader",nullptr,"shaders/textureVert.glsl", "shaders/textureFrag.glsl");
	AddComponent<ShaderComponent>("ShadowDepthShader",nullptr,"shaders/shadowDepthVert.glsl", "shaders/shadowDepthFrag.glsl");
	AddComponent<ShaderComponent>("DrawNormalsShader",nullptr,"shaders/drawNormalsVert.glsl", "shaders/drawNormalsFrag.glsl","shaders/drawNormalsGeom.glsl");
	AddComponent<ShaderComponent>("WireFrameShader",nullptr,"shaders/wireFrameVert.glsl", "shaders/wireFrameFrag.glsl","shaders/wireFrameGeom.glsl");

	AddComponent<MaterialComponent>("MarioMain",nullptr, "textures/mario_main.png");
	
	AddComponent<MaterialComponent>("ChessBoard",nullptr, "textures/8x8_checkered_board.png");
	AddComponent<MaterialComponent>("CheckerBoard",nullptr, "textures/8x8_checkered_board2.png");
	AddComponent<MaterialComponent>("BlackChessTexture", nullptr,"textures/Black Chess Base Colour.png");
	AddComponent<MaterialComponent>("WhiteChessTexture", nullptr,"textures/White Chess Base Colour.png");
	AddComponent<MaterialComponent>("RedCheckerTexture", nullptr,"textures/redCheckerPiece.png");
	AddComponent<MaterialComponent>("BlackCheckerTexture", nullptr, "textures/blackCheckerPiece.png");
	AddComponent<MaterialComponent>("WhiteCheckerTexture", nullptr, "textures/whiteCheckerPiece.png");


	OnCreate();
	ListAllComponents();
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

