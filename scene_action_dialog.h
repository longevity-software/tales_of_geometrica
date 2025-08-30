#ifndef SCENE_ACTION_DIALOG_H
#define SCENE_ACTION_DIALOG_H

#include "extensions/olcPGEX_QuickGUI.h"

#include "scene_action_interface.h"
#include "dialog_button.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>

class SceneActionDialog: public SceneActionInterface {

private:

    olc::QuickGUI::Label* _main_text_label;
    std::string _text;
    olc::QuickGUI::Label* _speaker_text_label;
    std::string _speaker;
    std::string _speaker_side;
    olc::Renderable * _speaker_image;

    std::vector<DialogButton> _buttons;

    bool _reset_action = true;
    bool _should_advance_to_next_dialog = false;

    const uint32_t DO_NOT_CHANGE_SCENE = 0xFFFFFFFF;
    uint32_t _next_scene_index = DO_NOT_CHANGE_SCENE;

public:

    SceneActionDialog(olc::QuickGUI::Label * main_text_label, 
                        std::string text, 
                        olc::QuickGUI::Label * speaker_text_label, 
                        std::string speaker,
                        std::string speaker_side,
                        olc::Renderable * speaker_image,
                        std::vector<DialogButton> buttons) {

        this->_main_text_label = main_text_label;
        this->_text = text;
        this->_speaker_text_label = speaker_text_label;
        this->_speaker = speaker;
        this->_speaker_side = speaker_side;
        this->_speaker_image = speaker_image;
        this->_buttons = buttons;

        for (uint32_t i = 0; i < this->_buttons.size(); ++i)
        {
            this->_buttons[i].pressed = false;
        }

        this->_reset_action = true;
        this->_should_advance_to_next_dialog = false;
    }

    void ResetAction() override {
        this->_reset_action = true;

        this->_next_scene_index = DO_NOT_CHANGE_SCENE;
        this->_should_advance_to_next_dialog = false;

        const uint32_t N_BUTTONS = this->_buttons.size();

        for (uint32_t i = 0; i < N_BUTTONS; ++i)
        {
            this->_buttons[i].button->bVisible = false;
            this->_buttons[i].pressed = false;
        }

        this->_main_text_label->bVisible = false;
        this->_speaker_text_label->bVisible = false;
    }

    bool PerformAction(olc::PixelGameEngine* pge, float elapsed_time) override {
        
        const int32_t SCREEN_WIDTH = pge->ScreenWidth();
        const int32_t SCREEN_HEIGHT = pge->ScreenHeight();

        const float dialogBoxPadding = 5;
        const float dialogBoxHeightPercent = 30;
        
        const float dialogHeight = (((SCREEN_HEIGHT * dialogBoxHeightPercent) / 100) - dialogBoxPadding);
        const float dialogBoxStart = ((SCREEN_HEIGHT - dialogHeight) - dialogBoxPadding);
    
        if (this->_reset_action)
        {
            this->_main_text_label->bVisible = true;
            this->_speaker_text_label->bVisible = true;

            this->_main_text_label->vPos = { dialogBoxPadding, dialogBoxStart };
            this->_main_text_label->vSize = { ((float)SCREEN_WIDTH - (2.0f * dialogBoxPadding)), dialogHeight };

            const float speakerBoxHeightPercent = 10;
            const float speakerBoxOffset = 2;

            const float TEXT_ELEMENT_HEIGHT = ((SCREEN_HEIGHT * speakerBoxHeightPercent) / 100);

            const float TEXT_ELEMENT_WIDTH = 75;

            this->_speaker_text_label->vPos = { (dialogBoxPadding - speakerBoxOffset), (dialogBoxStart - (uint32_t)TEXT_ELEMENT_HEIGHT + (2 * speakerBoxOffset)) };
            this->_speaker_text_label->vSize = {TEXT_ELEMENT_WIDTH, TEXT_ELEMENT_HEIGHT};

            this->_main_text_label->sText = this->_text;
            this->_speaker_text_label->sText = this->_speaker;

            const float BUTTON_X = (SCREEN_HEIGHT - TEXT_ELEMENT_HEIGHT);

            const uint32_t N_BUTTONS = this->_buttons.size();

            float button_y = 0;

            for (uint32_t i = 0; i < N_BUTTONS; ++i)
            {
                if ("right" == this->_buttons[i].side)
                {
                    button_y = (SCREEN_WIDTH - TEXT_ELEMENT_WIDTH - dialogBoxPadding - 3);
                }    
                else if ("left" == this->_buttons[i].side)
                {
                    button_y = (dialogBoxPadding + 3);
                }
                else
                {
                    continue;
                }
                
                this->_buttons[i].button->vPos = {button_y, BUTTON_X};
                this->_buttons[i].button->vSize = {TEXT_ELEMENT_WIDTH, TEXT_ELEMENT_HEIGHT};
                this->_buttons[i].button->sText = this->_buttons[i].text;
                this->_buttons[i].button->bVisible = true;
            }

            this->_reset_action = false;
        }

        const float CHARACTER_ORIGIN = (SCREEN_WIDTH / 2);

        const float CHARACTER_WIDTH = ("right" == this->_speaker_side) ? CHARACTER_ORIGIN : -CHARACTER_ORIGIN;

        pge->DrawPartialDecal({CHARACTER_ORIGIN,0},        // Position
            {CHARACTER_WIDTH,dialogBoxStart},              // Size
            this->_speaker_image->Decal(),
            {0.0f, 0.0f},
            {(float)this->_speaker_image->Decal()->sprite->width, (float)this->_speaker_image->Decal()->sprite->height});

        for (uint32_t i = 0; i < this->_buttons.size(); ++i)
        {
            if (this->_buttons[i].button->bPressed)
            {
                this->_buttons[i].pressed = true;
            }
            else if (this->_buttons[i].pressed)
            {
                // The button has been pressed, so perform the action
                if ("next_dialog" == this->_buttons[i].action)
                {
                    this->_should_advance_to_next_dialog = true;
                }
                else if (this->_buttons[i].action.compare(0, 6, "scene_") == 0)
                {
                    this->_next_scene_index = std::stoi(this->_buttons[i].action.substr(6));
                }

                this->_buttons[i].pressed = false;
            }
        }

        return false;
    }

    bool ShouldAdvanceToNextAction() {
        return this->_should_advance_to_next_dialog;
    }

    uint32_t ShouldAdvanceToScene() {
        return this->_next_scene_index;
    }
};

#endif // SCENE_ACTION_SET_BACKGROUND_H
