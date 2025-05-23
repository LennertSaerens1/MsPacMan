#include <SDL.h>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif
#include <Windows.h>
#include<Xinput.h>

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "Scene.h"
#include "RenderComponent.h"
#include "FpsComponent.h"
#include "RotationComponent.h"
#include "ImGuiComponent.h"
#include "Commands.h"
#include "InputManager.h"
#include "PacManCharacters.h"
#include "Displays.h"
#include "ServiceLocator.h"
#include "SoundSystem.h" 
#include "Utils.h"
#include "GridComponent.h"


void load()
{
	dae::ServiceLocator::RegisterSoundSystem(std::make_unique<dae::SoundSystem>());
	
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Demo");

	auto font = dae::ResourceManager::GetInstance().LoadFont("PacManFont.ttf", 36);

	auto go = std::make_shared<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>(std::string("SpriteSheet.png"),*go.get());
	go->SetWorldPosition(0, 100);
	auto renderComponent = go->getComponent<dae::RenderComponent>();
	renderComponent->SetSrcRect(utils::Rect{ 228, 0, 224, 248 });
	//renderComponent->SetSrcRect(utils::Rect{ 0, 248 *2, 224, 248 });
	renderComponent->SetDestRect(utils::Rect{0, 0, 224*3, 248*3});

	scene.Add(go);

	auto grid = std::make_shared<dae::GameObject>();
	grid->AddComponent<dae::GridComponent>(*grid.get(), 28, 31, 224 * 3, 248 * 3);
	grid->SetWorldPosition(0, 100);
	auto gridComponent = grid->getComponent<dae::GridComponent>();
	gridComponent->LoadGridFromFile("level1.txt");

	scene.Add(grid);

	{
		auto pacMan = std::make_shared<dae::GameObject>();
		auto pacManWorld = gridComponent->GetWorldCoordinatesMiddle(13, 23);
		pacMan->AddComponent<dae::PacManCharacter>(*pacMan.get(), gridComponent);

		pacMan->getComponent<dae::PacManCharacter>()->SetMiddlePosition(pacManWorld.x, pacManWorld.y);

		// Create commands for PacMan
		auto moveUp = std::make_shared<dae::MoveCommand>(dae::Direction::Up, pacMan.get());
		auto moveDown = std::make_shared<dae::MoveCommand>(dae::Direction::Down, pacMan.get());
		auto moveLeft = std::make_shared<dae::MoveCommand>(dae::Direction::Left, pacMan.get());
		auto moveRight = std::make_shared<dae::MoveCommand>(dae::Direction::Right, pacMan.get());

		// Bind PacMan movement to keyboard (WASD)
		dae::InputManager::GetInstance().BindKeyboardCommand(SDL_SCANCODE_W, moveUp);
		dae::InputManager::GetInstance().BindKeyboardCommand(SDL_SCANCODE_S, moveDown);
		dae::InputManager::GetInstance().BindKeyboardCommand(SDL_SCANCODE_A, moveLeft);
		dae::InputManager::GetInstance().BindKeyboardCommand(SDL_SCANCODE_D, moveRight);
		
		dae::InputManager::GetInstance().BindControllerCommand(XINPUT_GAMEPAD_DPAD_UP, moveUp);
		dae::InputManager::GetInstance().BindControllerCommand(XINPUT_GAMEPAD_DPAD_DOWN, moveDown);
		dae::InputManager::GetInstance().BindControllerCommand(XINPUT_GAMEPAD_DPAD_LEFT, moveLeft);
		dae::InputManager::GetInstance().BindControllerCommand(XINPUT_GAMEPAD_DPAD_RIGHT, moveRight);


		auto HighScoreText = std::make_shared<dae::GameObject>();
		HighScoreText->AddComponent<dae::TextComponent>("High Score:", font, *HighScoreText.get());
		HighScoreText->SetWorldPosition(200, 0);

		scene.Add(HighScoreText);

		int highScore = LoadHighScore();

		auto highScoreDisplay = std::make_shared<dae::GameObject>();
		highScoreDisplay->AddComponent<dae::TextComponent>(std::to_string(highScore), font, *highScoreDisplay.get());
		highScoreDisplay->SetWorldPosition(200, 50);

		scene.Add(highScoreDisplay);

		{
			/*auto livesDisplayP1 = std::make_shared<dae::GameObject>();
			livesDisplayP1->AddComponent<dae::TextComponent>("# Lives: 3", smallFont, *livesDisplayP1.get());
			livesDisplayP1->SetWorldPosition(0, 100);
			auto livesDisplayObserver = new dae::LivesDisplayObserver(livesDisplayP1.get());
			pacMan->AddObserver(livesDisplayObserver);

			scene.Add(livesDisplayP1);*/

			auto lives = std::make_shared<dae::GameObject>();
			lives->AddComponent<dae::RenderComponent>("HUD.png", *lives.get());
			lives->SetWorldPosition(50, 248 * 3 + 100);
			auto renderComponentLives = lives->getComponent<dae::RenderComponent>();
			renderComponentLives->SetSrcRect(utils::Rect{ 696, 96, 80, 16 });
			renderComponentLives->SetDestRect(utils::Rect{ 0, 0, 80 * 3, 16 * 3 });

			auto livesDisplayObserver = new dae::LivesDisplayObserver(lives.get());
			pacMan->AddObserver(livesDisplayObserver);

			scene.Add(lives);

			auto scoreDisplayP1 = std::make_shared<dae::GameObject>();
			scoreDisplayP1->AddComponent<dae::TextComponent>("0", font, *scoreDisplayP1.get());
			scoreDisplayP1->SetWorldPosition(50, 50);
			auto scoreDisplayObserver = new dae::ScoreDisplayObserver(scoreDisplayP1.get());
			pacMan->AddObserver(scoreDisplayObserver);

			scene.Add(scoreDisplayP1);

			auto damageCommand = std::make_shared<dae::DieCommand>(pacMan.get());
			auto scoreCommand100 = std::make_shared<dae::AddScoreCommand>(100, pacMan.get());
			auto scoreCommand200 = std::make_shared<dae::AddScoreCommand>(1000, pacMan.get());

			dae::InputManager::GetInstance().BindKeyboardDownCommand(SDL_SCANCODE_X, damageCommand);
			dae::InputManager::GetInstance().BindKeyboardDownCommand(SDL_SCANCODE_E, scoreCommand100);
			dae::InputManager::GetInstance().BindKeyboardDownCommand(SDL_SCANCODE_Q, scoreCommand200);
			

		}//Creating Lives and Score Display for PacMan
		scene.Add(pacMan);
	}//Creating PacMan and MsPacMan
}

int main(int, char*[]) {
	dae::InvincibleEngine engine("../Data/");
	engine.SetFrameRate(60);
	engine.Run(load);
    return 0;
}