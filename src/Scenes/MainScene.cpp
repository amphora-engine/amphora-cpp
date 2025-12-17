#include <iostream>
#include <ostream>

#include "Amphora.h"

using namespace Amphora;

class MainScene final : public Scene
{
	String welcomeText = String("Roboto", 32, 0, 0, 0, Colors::black, true, false,
		"Welcome to the Amphora Game Engine C++ Edition!");
	String quitText = String("Roboto", 18, 0, 0, -0, Colors::black, true, false,
		"Press ESC (or click this text) to quit");
	Vector2 screenCenter = GetRenderLogicalSize();

public:
	static MainScene& instance();

	void
	Init()
	override
	{
		SetBGColor(Colors::sky);

		Vector2 welcomeTextDimensions = welcomeText.Dimensions();
		Vector2 quitTextDimensions = quitText.Dimensions();
		welcomeText.SetPosition(static_cast<float>(screenCenter.x / 2 - welcomeTextDimensions.x / 2),
			static_cast<float>(screenCenter.y / 2 - welcomeTextDimensions.y));
		quitText.SetPosition(static_cast<float>(screenCenter.x / 2 - quitTextDimensions.x / 2),
			static_cast<float>(screenCenter.y / 2 + welcomeTextDimensions.y + 12));
		quitText.Hide();
	}

	void
	Update()
	override
	{
		bool welcomeTextShown = false;

		if (welcomeText.Typewriter(24) == TYPEWRITER_DONE)
		{
			welcomeTextShown = true;
		}
		if (welcomeTextShown)
		{
			quitText.Show();
			quitText.Typewriter(24);
		}

		if (Actions->Quit) QuitGame();
		if (quitText.Clicked(MouseLeftButton)) QuitGame();
	}

	void
	Destroy()
	override
	{}
};

Amphora_RegisterScene(MainScene);
