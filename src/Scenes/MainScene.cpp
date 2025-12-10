#include "Amphora.h"

using namespace Amphora;

class MainScene final : public Scene
{
	AmphoraString *welcomeText = nullptr;
	AmphoraString *quitText = nullptr;
	Vector2 screenCenter = GetRenderLogicalSize();

public:
	static MainScene& instance();

	void
	init()
	override
	{
		SetBGColor(Colors::sky);

		welcomeText = CreateString("Roboto", 32, 0, 0, 0, Colors::black, true,
			"Welcome to the Amphora Game Engine C++ Edition!");
		quitText = CreateString("Roboto", 18, 0, 0, -0, Colors::black, true,
			"Press ESC (or click this text) to quit");

		Vector2 welcomeTextDimensions = GetStringDimensions(welcomeText);
		Vector2 quitTextDimensions = GetStringDimensions(quitText);
		UpdateStringPosition(welcomeText,
			static_cast<float>(screenCenter.x / 2 - welcomeTextDimensions.x / 2),
			static_cast<float>(screenCenter.y / 2 - welcomeTextDimensions.y));
		UpdateStringPosition(quitText,
			static_cast<float>(screenCenter.x / 2 - quitTextDimensions.x / 2),
			static_cast<float>(screenCenter.y / 2 + welcomeTextDimensions.y + 12));
		HideString(quitText);
	}

	void
	update(unsigned int frame, const InputState* input)
	override
	{
		bool welcomeTextShown = false;

		if (TypeString(welcomeText, 24, nullptr) == TYPEWRITER_DONE)
		{
			welcomeTextShown = true;
		}
		if (welcomeTextShown)
		{
			ShowString(quitText);
			TypeString(quitText, 24, nullptr);
		}

		if (input->quit) QuitGame();
		if (ObjectClicked(quitText, MouseLeftButton, nullptr)) QuitGame();
	}

	void
	destroy()
	override
	{
		welcomeText = nullptr;
		quitText = nullptr;
	}
};

Amphora_RegisterScene(MainScene);
