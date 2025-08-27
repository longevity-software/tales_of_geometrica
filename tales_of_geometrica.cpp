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
#include "scene_action_shaking_dialog.h"
#include "dialog_button.h"

#include "json.h"

class TalesOfGeometrica : public olc::PixelGameEngine
{
protected:

	std::map<std::string, olc::Renderable> _characters;
	std::map<std::string, olc::Renderable> _backgrounds;

	// UI Components
	olc::QuickGUI::Manager _guiManager;
	olc::QuickGUI::Label* _textLabel = nullptr;
	olc::QuickGUI::Label* _speakerLabel = nullptr;

	std::vector<olc::QuickGUI::Button*> _buttons;

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
		_characters["Mathilda"] = olc::Renderable();
		_characters["Mathilda"].Load("assets/characters/Mathilda.png");
		_characters["Oscar"] = olc::Renderable();
		_characters["Oscar"].Load("assets/characters/Oscar.png");
		_characters["Charlie"] = olc::Renderable();
		_characters["Charlie"].Load("assets/characters/Charlie.png");
		_characters["Dr Robinson"] = olc::Renderable();
		_characters["Dr Robinson"].Load("assets/characters/Dr Robinson.png");

		// load the backgrounds
		_backgrounds["winding_road"] = olc::Renderable();
		_backgrounds["winding_road"].Load("assets/backgrounds/winding_road.png");

		_backgrounds["side_bridge"] = olc::Renderable();
		_backgrounds["side_bridge"].Load("assets/backgrounds/side_bridge.png");

		_textLabel = new olc::QuickGUI::Label(_guiManager,
			"", { 0, 0 }, { 0, 0 });
	
		_textLabel->nAlign = olc::QuickGUI::Label::Alignment::Centre;
		_textLabel->bHasBorder = true;
		_textLabel->bHasBackground = true;
		
		_speakerLabel = new olc::QuickGUI::Label(_guiManager, "", { 0, 0 }, {100, 10});
		_speakerLabel->nAlign = olc::QuickGUI::Label::Alignment::Centre;
		_speakerLabel->bHasBorder = true;
		_speakerLabel->bHasBackground = true;

		// set up 2 buttons
		_buttons.push_back(new olc::QuickGUI::Button(_guiManager, "", {0,0}, {0,0}));
		_buttons.push_back(new olc::QuickGUI::Button(_guiManager, "", {0,0}, {0,0}));

		int scene_index = 0;
		bool scene_found = true;

		do
		{
			std::ifstream f("./assets/scenes/" + std::to_string(scene_index) + ".togs");

			scene_found = f.good();

			if (scene_found)
			{
				// Add a new scene
				const uint32_t SCENE_INDEX = _scenes.size();
				_scenes.push_back(Scene(SCENE_INDEX));

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
					else if (("dialog" == action) || ("shaking_dialog" == action))
					{
						std::string text = result.array(i).get("text").as_string();
						std::string speaker = result.array(i).get("speaker").as_string();
						std::string speaker_side = result.array(i).get("location").as_string();
						olc::Renderable * speaker_image = &_characters[speaker];

						json::jobject buttons_json = result.array(i).get("buttons");

						std::vector<DialogButton> buttons;

						for (int b = 0; b < buttons_json.size(); ++b)
						{
							DialogButton db = DialogButton();

							db.side = buttons_json.array(b).get("side").as_string();
							db.text = buttons_json.array(b).get("text").as_string();
							db.action = buttons_json.array(b).get("action").as_string();

							if ("left" == db.side)
							{
								db.button = _buttons[0];
							}
							else
							{
								db.button = _buttons[1];
							}

							buttons.push_back(db);
						}

						if ("dialog" == action)
						{
							_scenes[scene_index].AddSceneAction(std::make_unique<SceneActionDialog>(_textLabel, 
																									text, 
																									_speakerLabel, 
																									speaker, 
																									speaker_side,
																									speaker_image,
																									buttons));
						}
						else
						{
							_scenes[scene_index].AddSceneAction(std::make_unique<SceneActionShakingDialog>(_textLabel, 
																									text, 
																									_speakerLabel, 
																									speaker, 
																									speaker_side,
																									speaker_image,
																									buttons));							
						}
					}
				}

				scene_index++;
			}
			
		}while(scene_found);

		_current_scene_index = 0;

		_textLabel->sText = "This is a test.";

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		_guiManager.Update(this);

        Clear(olc::BLACK);

		_scenes[_current_scene_index].DrawScene(this, fElapsedTime);

		_guiManager.DrawDecal(this);

		const uint32_t NEXT_SCENE = _scenes[_current_scene_index].NextScene();

		if (NEXT_SCENE != _current_scene_index)
		{
			// reset the current scene
			_scenes[_current_scene_index].ResetScene();

			// change the scene
			// TODO - Validate this
			_current_scene_index = NEXT_SCENE;
		}

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