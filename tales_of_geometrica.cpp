#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// PGEX Require the presence of olc::PixelGameEngine
#define OLC_PGEX_QUICKGUI
#include "extensions/olcPGEX_QuickGUI.h"

#include <string>
#include <fstream>
#include <iterator>
#include <map>
#include <vector>
#include <memory>

#include "scene.h"
#include "scene_action_interface.h"
#include "scene_action_dialog.h"

#include "json.h"

class TalesOfGeometrica : public olc::PixelGameEngine
{
protected:

    olc::Renderable _mathildaDialogueSprite;

	std::map<std::string, olc::Renderable> _backgrounds;

	// UI Components
	olc::QuickGUI::Manager guiManager;
	olc::QuickGUI::Label* textLabel = nullptr;
	olc::QuickGUI::Label* speakerLabel = nullptr;

	std::vector<Scene> _scenes;

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

		textLabel = new olc::QuickGUI::Label(guiManager,
			"", { 0, 0 }, { 0, 0 });
	
		textLabel->nAlign = olc::QuickGUI::Label::Alignment::Centre;
		textLabel->bHasBorder = true;
		// textLabel->bVisible = false;
		textLabel->bHasBackground = true;
		
		speakerLabel = new olc::QuickGUI::Label(guiManager, "", { 0, 0 }, {100, 10});
		speakerLabel->nAlign = olc::QuickGUI::Label::Alignment::Centre;
		speakerLabel->bHasBorder = true;
		speakerLabel->bHasBackground = true;

		int scene_index = 0;
		bool scene_found = true;

		do
		{
			std::ifstream f("./assets/scenes/" + std::to_string(scene_index) + ".togs");

			scene_found = f.good();

			if (scene_found)
			{
				// Add a new scene
				_scenes.push_back(Scene());

				std::string content((std::istreambuf_iterator<char>(f)),
				std::istreambuf_iterator<char>());

				json::jobject result = json::jobject::parse(content);

				for (int i = 0; i < result.size(); ++i)
				{
					std::string action = result.array(i).get("action").as_string();

					if ("background" == action)
					{
						std::string background_image = result.array(i).get("image").as_string();

						_scenes[scene_index].AddBackgroundImage(&_backgrounds[background_image]);
					}
					else if ("dialog" == action)
					{
						std::string text = result.array(i).get("text").as_string();
						std::string speaker = result.array(i).get("speaker").as_string();

						_scenes[scene_index].AddSceneAction(std::make_unique<SceneActionDialog>(textLabel, text, speakerLabel, speaker));
					}
				}

				scene_index++;
			}
			
		}while(scene_found);

		_current_scene_index = 0;

		textLabel->sText = "This is a test.";

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		guiManager.Update(this);

        Clear(olc::BLACK);

		_scenes[_current_scene_index].DrawScene(this);
        
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