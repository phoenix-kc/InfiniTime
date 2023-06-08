#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H


#include <vector>

#include "Screen.h"
#include "ScreenGraph.h"



class SettingsScreen : public Screen
{
public:

        SettingsScreen(ScreenGraph *screenGraph, ComponentContainer *components);
        ~SettingsScreen() override;

        lv_color_t foregroundColor() const override;

        bool isMultiPageScreen() const override { return true; }
        uint8_t pageCount() const override { return _pages; }

protected:

        void handleCurrentPageChange() override;

private:

        struct ButtonInfo
        {
        public:
                uint8_t page;
                uint8_t row;
                const lv_font_t *iconFont;
                const char *icon;
                const char *text;
                ScreenGraph::ScreenTag destScreen;
                SettingsScreen *screen;
        };


        std::vector<ButtonInfo *> _buttons;
        uint8_t _pages;

        void addButton(const lv_font_t *iconFont, const char *icon, const char *text, ScreenGraph::ScreenTag destScreen);

        void buttonClicked(int row);

        static void buttonCallback(lv_obj_t *object, lv_event_t event);
};

#endif // SETTINGSSCREEN_H
