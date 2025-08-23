#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// PGEX Require the presence of olc::PixelGameEngine
#define OLC_PGEX_QUICKGUI
#include "extensions/olcPGEX_QuickGUI.h"

#include <string>
#include <fstream>
#include <iterator>
#include <map>
#include <deque>
#include <memory>

#include "scene_action_interface.h"
#include "scene_action_set_background.h"

#include "json.h"

class TalesOfGeometrica : public olc::PixelGameEngine
{
protected:

    olc::Renderable _mathildaDialogueSprite;

	std::map<std::string, olc::Renderable> _backgrounds;

	// UI Components
	olc::QuickGUI::Manager guiManager;
	olc::QuickGUI::Label* guiLabel = nullptr;

	const float dialogBoxPadding = 5;
	const float dialogBoxHeightPercent = 30;

	std::map<int, std::deque<std::unique_ptr<SceneActionInterface>>> _scenes;

	int _current_scene_index;

public:
	const int32_t windowWidth = 240;
	const int32_t windowHeight = 160;

public:
	TalesOfGeometrica()
	{
		sAppName = "Tales of Geometrica";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
        _mathildaDialogueSprite.Load("assets/gfx/MathildaBaseDialogue.png");

		// load the backgrounds
		_backgrounds["winding_road"] = olc::Renderable();
		_backgrounds["winding_road"].Load("assets/backgrounds/winding_road.png");

		const float dialogHeight = (((windowHeight * dialogBoxHeightPercent) / 100) - dialogBoxPadding);
		const float dialogBoxStart = ((windowHeight - dialogHeight) - dialogBoxPadding);

		guiLabel = new olc::QuickGUI::Label(guiManager,
			"", { dialogBoxPadding, dialogBoxStart }, { ((float)windowWidth - (2.0f * dialogBoxPadding)), dialogHeight });
	
		guiLabel->nAlign = olc::QuickGUI::Label::Alignment::Centre;
		guiLabel->bHasBorder = true;
		// guiLabel->bVisible = false;
		guiLabel->bHasBackground = true;

		int scene_index = 0;
		bool scene_found = true;

		do
		{
			std::ifstream f("./assets/scenes/" + std::to_string(scene_index) + ".togs");

			scene_found = f.good();

			if (scene_found)
			{
				_scenes[scene_index] = std::deque<std::unique_ptr<SceneActionInterface>>();

				std::string content((std::istreambuf_iterator<char>(f)),
				std::istreambuf_iterator<char>());

				json::jobject result = json::jobject::parse(content);

				for (int i = 0; i < result.size(); ++i)
				{
					std::string action = result.array(i).get("action").as_string();

					if ("background" == action)
					{
						std::string background_image = result.array(i).get("image").as_string();

						_scenes[i].push_back(std::make_unique<SceneActionSetBackground>(&_backgrounds[background_image]));
					}
				}

				scene_index++;
			}
			
		}while(scene_found);

		_current_scene_index = 0;

		guiLabel->sText = "This is a test.";

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		guiManager.Update(this);

        Clear(olc::BLACK);

		if (!_scenes[_current_scene_index].empty())
		{
			_scenes[_current_scene_index].front()->PerformAction(this);
		}
        
		// if (!_scene_actions.empty())
		// {
		// 	if (_scene_actions.front()->PerformAction(this))
		// 	{
		// 		_scene_actions.pop_front();
		// 	}
		// }

        // DrawDecal(GetMousePos(), _mathildaDialogueSprite.Decal());

		guiManager.DrawDecal(this);

		return true;
	}
};

int main()
{
	TalesOfGeometrica game;
	if (game.Construct(game.windowWidth, game.windowHeight, 4, 4))
		game.Start();

	return 0;
}