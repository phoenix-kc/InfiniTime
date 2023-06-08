#ifndef WAKEUPMODESCREEN_H
#define WAKEUPMODESCREEN_H


#include <vector>

#include "Screen.h"



class WakeUpModeScreen : public Screen
{
public:

        WakeUpModeScreen(ScreenGraph *screenGraph, ComponentContainer *components);
        ~WakeUpModeScreen() override;

private:

        struct OptionInfo
        {
        public:
                WakeUpModeScreen *screen;
                uint8_t row;
                lv_obj_t *checkBoxLabel;
        };

        std::vector<OptionInfo *> _optionInfos;

        void addOption(const char *text);
        void updateCheckBoxes();
        void optionClicked(int row);

        static void buttonCallback(lv_obj_t *object, lv_event_t event);
};

#endif // WAKEUPMODESCREEN_H
