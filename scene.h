#ifndef _SCENE_H
#define _SCENE_H

#include "olcPixelGameEngine.h"

#include "scene_action_interface.h"

#include <vector>
#include <memory>

class Scene {

    private:

        olc::Renderable * _background_image;

        std::vector<std::unique_ptr<SceneActionInterface>> _scene_actions = std::vector<std::unique_ptr<SceneActionInterface>>();

        int _scene_action_index;

        uint32_t _this_scenes_index = 0;

        uint32_t _next_scenes_index = 0;

        bool _first_draw = true;
    
    public:

        Scene(uint32_t index) {
            this->_scene_action_index = 0;
            this->_this_scenes_index = index;
            this->_next_scenes_index = index;
            this->_first_draw = true;
        }

        void ResetScene() {
            this->_scene_action_index = 0;

            for (uint32_t i = 0; i < this->_scene_actions.size(); ++i)
            {
                this->_scene_actions[i]->ResetAction();
            }

            // reset the next scene index
            this->_next_scenes_index = this->_this_scenes_index;

            this->_first_draw = true;
        }

        void AddBackgroundImage(olc::Renderable * background_image) {
            this->_background_image = background_image;
        }

        void AddSceneAction(std::unique_ptr<SceneActionInterface> scene_action_ptr) {
            this->_scene_actions.push_back(std::move(scene_action_ptr));
        }

        uint32_t NextScene() {
            return this->_next_scenes_index;
        }

        void DrawScene(olc::PixelGameEngine* pge, float elapsed_time) {
  
            if (this->_first_draw)
            {
                std::cout << "First Draw " + std::to_string(this->_this_scenes_index) + "\n";

                this->_first_draw = false;
            }   

            pge->DrawDecal(olc::vf2d(0,0), this->_background_image->Decal());

            this->_scene_actions[this->_scene_action_index]->PerformAction(pge, elapsed_time);

            if (this->_scene_actions[this->_scene_action_index]->ShouldAdvanceToNextAction())
            {
                // reset the current action
                this->_scene_actions[this->_scene_action_index]->ResetAction();

                // TODO - Add validation around this
                this->_scene_action_index++;
            }
            else
            {
                const uint32_t NEXT_SCENE = this->_scene_actions[this->_scene_action_index]->ShouldAdvanceToScene();

                if (0xFFFFFFFF != NEXT_SCENE)
                {
                    // reset the current action
                    this->_scene_actions[this->_scene_action_index]->ResetAction();

                    this->_next_scenes_index = NEXT_SCENE;
                }
            }
        }
};

#endif // _SCENE_H
