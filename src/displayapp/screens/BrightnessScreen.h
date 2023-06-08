#ifndef BRIGHTNESSSCREEN_H
#define BRIGHTNESSSCREEN_H


#include <vector>

#include "Screen.h"



class BrightnessScreen : public Screen
{
public:

        BrightnessScreen(ScreenGraph *screenGraph, ComponentContainer *components);
        ~BrightnessScreen() override;

private:

        struct OptionInfo
        {
        public:
                BrightnessScreen *screen;
                uint8_t row;
                lv_obj_t *checkBoxLabel;
        };

        std::vector<OptionInfo *> _optionInfos;

        void addOption(lv_font_t *iconFont, const char *icon, const char *text);
        void updateCheckBoxes();
        void optionClicked(int row);

        static void buttonCallback(lv_obj_t *object, lv_event_t event);
};

#endif // BRIGHTNESSSCREEN_H
