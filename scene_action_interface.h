#ifndef _SCENE_ACTION_INTERFACE_H
#define _SCENE_ACTION_INTERFACE_H

#include "olcPixelGameEngine.h"

class SceneActionInterface {

public:
    virtual bool PerformAction(olc::PixelGameEngine* pge) = 0;
};

#endif // _SCENE_ACTION_INTERFACE_H
