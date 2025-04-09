#include <SDL.h>
#include "SceneManager.h"
#include "Timer.h"
#include "Window.h"
#include "Scene0.h"
#include "Scene1.h"
#include "Scene2.h"
#include "Scene3.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

// Initialize static instance
SceneManager* SceneManager::instance = nullptr;

// Singleton function to get the instance
SceneManager* SceneManager::GetInstance() {
	if (instance == nullptr) {
		instance = new SceneManager();
	}
	return instance;
}



SceneManager::SceneManager(): 
	currentScene(nullptr), window(nullptr), timer(nullptr),
	fps(60), isRunning(false), fullScreen(false), show_demo_window(true), show_another_window(true) {
	Debug::Info("Starting the SceneManager", __FILE__, __LINE__);
}

SceneManager::~SceneManager() {
	if (currentScene) {
		delete currentScene;
		currentScene = nullptr;
	}
	
	if (timer) {
		delete timer;
		timer = nullptr;
	}

	if (window) {
		delete window;
		window = nullptr;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	Debug::Info("Deleting the SceneManager", __FILE__, __LINE__);
}

bool SceneManager::Initialize(std::string name_, int width_, int height_) {

	window = new Window();
	if (!window->OnCreate(name_, width_, height_)) {
		Debug::FatalError("Failed to initialize Window object", __FILE__, __LINE__);
		return false;
	}

	timer = new Timer();
	if (timer == nullptr) {
		Debug::FatalError("Failed to initialize Timer object", __FILE__, __LINE__);
		return false;
	}

	/********************************   Default first scene   ***********************/
	BuildNewScene(SCENE_NUMBER::SCENE3);
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window->getWindow(), window->getContext());
	ImGui_ImplOpenGL3_Init("#version 450");

	return true;
}


/// This is the whole game
void SceneManager::Run() {
	timer->Start();
	isRunning = true;
	while (isRunning) {
		timer->UpdateFrameTicks();
		

		if (dead) {
			BuildNewScene(SCENE_NUMBER::SCENE2);
			dead = false;
			
		}
		if (triggerScene1) {
			BuildNewScene(SCENE_NUMBER::SCENE1);
			triggerScene1 = false; // reset the flag
		}


		currentScene->Update(timer->GetDeltaTime());

		HandleEvents();
		currentScene->Render();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if (dynamic_cast<Scene2*>(currentScene)) {
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

			ImGui::Begin("##InvisibleWindow", nullptr,
				ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 buttonSize = ImVec2(300, 100);

			// Lower the buttons vertically
			ImVec2 buttonRowPos = ImVec2((windowSize.x - (buttonSize.x * 2 + 20)) * 0.5f,  // horizontally centered across two buttons + spacing
				(windowSize.y * 0.78f));  // Lowered vertically (adjust 0.65 to taste)

			ImGui::SetCursorPos(buttonRowPos);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));
			ImGui::SetWindowFontScale(3);

			// Retry Button
			if (ImGui::Button("Retry", buttonSize)) {
				BuildNewScene(SCENE_NUMBER::SCENE1);
			}

			ImGui::SameLine(); // Makes the next widget appear on the same row

			// Main Menu Button
			if (ImGui::Button("Main Menu", buttonSize)) {
				BuildNewScene(SCENE_NUMBER::SCENE0);
			}

			ImGui::PopStyleVar();
			ImGui::End();
		}


		// Health bar rendering
		if (dynamic_cast<Scene1*>(currentScene) || dynamic_cast<Scene3*>(currentScene))
		{
			
			ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always); 
			ImGui::SetNextWindowSize(ImVec2(300, 50), ImGuiCond_Always);
			ImGui::Begin("Health Bar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			float healthPercentage = playerHealth / maxHealth;
			ImGui::Text("Health");
			ImGui::ProgressBar(healthPercentage, ImVec2(-1.0f, 0.0f));
			ImGui::SetWindowFontScale(1);// text size
			ImGui::End();
		}

	if (dynamic_cast<Scene1*>(currentScene) || dynamic_cast<Scene3*>(currentScene))
	{
			ImGui::SetNextWindowPos(ImVec2(10, 70), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(250, 50), ImGuiCond_Always);

			ImGui::Begin("Ammo Counter", nullptr,
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse);

			ImGui::SetWindowFontScale(2.5); // Text size
			ImGui::Text("Ammo: %d/%d", clipAmmo, totalAmmo);
			ImGui::End();
		}


		if (dynamic_cast<Scene0*>(currentScene)) {
			// Set up a full-screen, borderless window for the button
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

			ImGui::Begin("##InvisibleWindow", nullptr,
				ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			// Get window size (full screen)
			ImVec2 windowSize = ImGui::GetWindowSize();

			// Button size
			ImVec2 buttonSize = ImVec2(400, 100); // Large button but fits on screen

			// Centering the button
			ImVec2 cursorPosStart = ImVec2((windowSize.x - buttonSize.x) * 0.5f,
				(windowSize.y - buttonSize.y) * 0.5f);
			ImVec2 cursorPosQuit = ImVec2((windowSize.x - buttonSize.x) * 0.5f,
				((windowSize.y - -300) - buttonSize.y) * 0.5f);

			

			ImGui::SetCursorPos(cursorPosStart);

			// Bigger button
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));

			if (ImGui::Button("Start Game", buttonSize)) {
				BuildNewScene(SCENE_NUMBER::SCENE1);
				engine->play2D("./Audio/SciButtonSound.mp3");
				engine->setSoundVolume(0.1);
				
			}

			ImGui::SetCursorPos(cursorPosQuit);
		
			if (ImGui::Button("Quit Game", buttonSize)) {
				isRunning = false;
			}
			ImGui::SetWindowFontScale(3);
			ImGui::PopStyleVar(); // Restore padding
			ImGui::End();
		}

		

		// Render ImGui draw data
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window->getWindow());
		SDL_Delay(timer->GetSleepTime(fps));
	}
}

void SceneManager::HandleEvents() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		if (sdlEvent.type == SDL_EventType::SDL_QUIT) {
			isRunning = false;
			return;
		}
		else if (sdlEvent.type == SDL_KEYDOWN) {
			switch (sdlEvent.key.keysym.scancode) {
			case SDL_SCANCODE_ESCAPE:
				isRunning = false;
				return;
				[[fallthrough]]; /// C17 Prevents switch/case fallthrough warnings

			case SDL_SCANCODE_F1:
				BuildNewScene(SCENE_NUMBER::SCENE1);
				break;

			case SDL_SCANCODE_F2:
				BuildNewScene(SCENE_NUMBER::SCENE2);
				break;

			case SDL_SCANCODE_F11:
				BuildNewScene(SCENE_NUMBER::SCENE0);
				break;
			}
		}
		if (currentScene == nullptr) {
			Debug::FatalError("Failed to initialize Scene", __FILE__, __LINE__);
			isRunning = false;
			return;
		}
		currentScene->HandleEvents(sdlEvent);
	}
}

void SceneManager::BuildNewScene(SCENE_NUMBER scene) {
	bool status; 

	if (currentScene != nullptr) {
		currentScene->OnDestroy();
		delete currentScene;
		currentScene = nullptr;
	}

	switch (scene) {

	
	case SCENE_NUMBER::SCENE0:
		currentScene = new Scene0();
		status = currentScene->OnCreate();
		break;
		

	case SCENE_NUMBER::SCENE1:
		currentScene = new Scene1(this);
		status = currentScene->OnCreate();
		break;

	case SCENE_NUMBER::SCENE2:
		currentScene = new Scene2();
		status = currentScene->OnCreate();
		break;

	case SCENE_NUMBER::SCENE3:
		currentScene = new Scene3(this);
		status = currentScene->OnCreate();
		break;

	default:
		Debug::Error("Incorrect scene number assigned in the manager", __FILE__, __LINE__);
		currentScene = nullptr;
		break;
	}	
}


