#ifndef DIALOG_BUTTON_H
#define DIALOG_BUTTON_H

class DialogButton {

    public:

        olc::QuickGUI::Button* button;
        std::string side;
        std::string text;
        std::string action;
        bool pressed;
};

#endif
