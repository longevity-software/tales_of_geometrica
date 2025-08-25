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

        bool _test;
    
    public:

        Scene() {
            this->_scene_action_index = 0;
            _test = true;
        }

        void ResetScene() {
            this->_scene_action_index = 0;
        }

        void AddBackgroundImage(olc::Renderable * background_image) {
            this->_background_image = background_image;
        }

        void AddSceneAction(std::unique_ptr<SceneActionInterface> scene_action_ptr) {
            this->_scene_actions.push_back(std::move(scene_action_ptr));
        }

        void DrawScene(olc::PixelGameEngine* pge) {            
            pge->DrawDecal(olc::vf2d(0,0), this->_background_image->Decal());

            this->_scene_actions[this->_scene_action_index]->PerformAction(pge);

            if (this->_scene_actions[this->_scene_action_index]->ShouldAdvanceToNextAction())
            {
                // reset the current action
                this->_scene_actions[this->_scene_action_index]->ResetAction();

                // TODO - Add validation around this
                this->_scene_action_index++;
            }
        }

};

#endif // _SCENE_H
