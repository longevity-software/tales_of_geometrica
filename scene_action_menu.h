#ifndef SCENE_ACTION_MENU_H
#define SCENE_ACTION_MENU_H

#include "extensions/olcPGEX_QuickGUI.h"

#include "scene_action_interface.h"

#include "vector"

class MenuButton {

    public:

        olc::QuickGUI::Button* button;
        std::string text;
        std::string action;
        bool pressed;
};

class SceneActionMenu: public SceneActionInterface {

private:

    std::vector<MenuButton> _buttons;

    bool _reset_action = true;
    
    const uint32_t DO_NOT_CHANGE_SCENE = 0xFFFFFFFF;
    uint32_t _next_scene_index = DO_NOT_CHANGE_SCENE;

public: 

    SceneActionMenu(std::vector<MenuButton> buttons) {
        this->_buttons = buttons;

        for (uint32_t i = 0; i < this->_buttons.size(); ++i)
        {
            this->_buttons[i].pressed = false;
        }

        this->_reset_action = true;

        this->_next_scene_index = DO_NOT_CHANGE_SCENE;
    }

    
    void ResetAction() override {
        this->_reset_action = true;

        this->_next_scene_index = DO_NOT_CHANGE_SCENE;

        const uint32_t N_BUTTONS = this->_buttons.size();

        for (uint32_t i = 0; i < N_BUTTONS; ++i)
        {
            this->_buttons[i].button->bVisible = false;
            this->_buttons[i].pressed = false;
        }
    }

    bool PerformAction(olc::PixelGameEngine* pge, float elapsed_time) override {
        
        const int32_t SCREEN_WIDTH = pge->ScreenWidth();
        const int32_t SCREEN_HEIGHT = pge->ScreenHeight();

        if (this->_reset_action)
        {
            const uint32_t BUTTON_WIDTH = 100;
            const uint32_t BUTTON_HEIGHT = 30;
            const uint32_t BUTTON_SPACING = 20;
            const uint32_t FIRST_BUTTON_Y = 120;
            const float BUTTON_X = (float)((SCREEN_WIDTH / 2) - (BUTTON_WIDTH / 2));
            const uint32_t N_BUTTONS = this->_buttons.size();

            for (uint32_t i = 0; i < N_BUTTONS; ++i)
            {
                const float BUTTON_Y = (float)(FIRST_BUTTON_Y + ((BUTTON_HEIGHT + BUTTON_SPACING) * i));
                this->_buttons[i].button->vPos = {BUTTON_X, BUTTON_Y};
                this->_buttons[i].button->vSize = {BUTTON_WIDTH, BUTTON_HEIGHT};
                this->_buttons[i].button->sText = this->_buttons[i].text;
                this->_buttons[i].button->bVisible = true;
            }

            this->_reset_action = false;
        }

        for (uint32_t i = 0; i < this->_buttons.size(); ++i)
        {
            if (this->_buttons[i].button->bPressed)
            {
                this->_buttons[i].pressed = true;
            }
            else if (this->_buttons[i].pressed)
            {
                // The button has been pressed, so perform the action
                if (this->_buttons[i].action.compare(0, 6, "scene_") == 0)
                {
                    this->_next_scene_index = std::stoi(this->_buttons[i].action.substr(6));
                }

                this->_buttons[i].pressed = false;
            }
        }

        return false;
    }

    bool ShouldAdvanceToNextAction() override {
        return false;
    }

    uint32_t ShouldAdvanceToScene() override {
        return this->_next_scene_index;
    }
};

#endif // SCENE_ACTION_MENU_H