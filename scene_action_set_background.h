#ifndef SCENE_ACTION_SET_BACKGROUND_H
#define SCENE_ACTION_SET_BACKGROUND_H

#include "scene_action_interface.h"

class SceneActionSetBackground: public SceneActionInterface {

public:

    SceneActionSetBackground(olc::Renderable * background) {
        this->_background = background;
    }

    bool PerformAction(olc::PixelGameEngine* pge) override {

        pge->DrawDecal(olc::vf2d(0,0), this->_background->Decal());

        return false;
    }

private:

    olc::Renderable * _background;

};

#endif // SCENE_ACTION_SET_BACKGROUND_H
