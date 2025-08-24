#ifndef SCENE_ACTION_DIALOG_H
#define SCENE_ACTION_DIALOG_H

#include "extensions/olcPGEX_QuickGUI.h"

#include "scene_action_interface.h"

#include <string>
#include <vector>
#include <map>

class DialogButton {

    public:

        olc::QuickGUI::Button* button;
        std::string side;
        std::string text;
        std::string action;
};

class SceneActionDialog: public SceneActionInterface {

private:

    olc::QuickGUI::Label* _main_text_label;
    std::string _text;
    olc::QuickGUI::Label* _speaker_text_label;
    std::string _speaker;

    std::vector<DialogButton> _buttons;

    bool _reset_action = true;

public:

    SceneActionDialog(olc::QuickGUI::Label * main_text_label, 
                        std::string text, 
                        olc::QuickGUI::Label * speaker_text_label, 
                        std::string speaker,
                        std::vector<DialogButton> buttons) {

        this->_main_text_label = main_text_label;
        this->_text = text;
        this->_speaker_text_label = speaker_text_label;
        this->_speaker = speaker;
        this->_buttons = buttons;

        this->_reset_action = true;
    }

    void ResetAction() override {
        this->_reset_action = true;
    }

    bool PerformAction(olc::PixelGameEngine* pge) override {
        
        if (this->_reset_action)
        {
            const int32_t SCREEN_WIDTH = pge->ScreenWidth();
            const int32_t SCREEN_HEIGHT = pge->ScreenHeight();
            
            const float dialogBoxPadding = 5;
            const float dialogBoxHeightPercent = 30;

            const float dialogHeight = (((SCREEN_HEIGHT * dialogBoxHeightPercent) / 100) - dialogBoxPadding);
            const float dialogBoxStart = ((SCREEN_HEIGHT - dialogHeight) - dialogBoxPadding);

            this->_main_text_label->vPos = { dialogBoxPadding, dialogBoxStart };
            this->_main_text_label->vSize = { ((float)SCREEN_WIDTH - (2.0f * dialogBoxPadding)), dialogHeight };

            const float speakerBoxHeightPercent = 10;
            const float speakerBoxOffset = 2;

            const float speakerBoxHeight = ((SCREEN_HEIGHT * speakerBoxHeightPercent) / 100);

            this->_speaker_text_label->vPos = { (dialogBoxPadding - speakerBoxOffset), (dialogBoxStart - (uint32_t)speakerBoxHeight + (2 * speakerBoxOffset)) };
            this->_speaker_text_label->vSize = {75, speakerBoxHeight};

            this->_main_text_label->sText = this->_text;
            this->_speaker_text_label->sText = this->_speaker;

            // TODO - populate the buttons

            this->_reset_action = false;
        }

        return false;
    }

};

#endif // SCENE_ACTION_SET_BACKGROUND_H
