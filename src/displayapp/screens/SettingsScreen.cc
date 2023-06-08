#include "SettingsScreen.h"

#include <lvgl/lvgl.h>

#include "displayapp/screens/ScreenGraph.h"
#include "displayapp/fonts/font_dvsb_ascii_18.h"
#include "displayapp/fonts/font_dvs_ascii_16.h"
#include "displayapp/fonts/FontAwesomeRegular24.h"
#include "displayapp/fonts/FontAwesomeSolid24.h"


#define BUTTON_HEIGHT      64
#define BUTTONS_START      44
#define BUTTONS_PER_PAGE   3



SettingsScreen::SettingsScreen(ScreenGraph *screenGraph, ComponentContainer *components)
        : Screen(screenGraph, components)
{
        // init
        _buttons.clear();
        _pages = 1;

        // add the buttons
        addButton(&FontAwesomeRegular24, SYMBOL_SUN, "Brightness", ScreenGraph::ScreenTag::Brightness);
        addButton(&FontAwesomeSolid24, SYMBOL_BED, "Wake up", ScreenGraph::ScreenTag::WakeUpMode);
        addButton(&FontAwesomeSolid24, SYMBOL_WALKING, "Activity goal", ScreenGraph::ScreenTag::StepsGoal);
        addButton(&FontAwesomeSolid24, SYMBOL_INFO, "System info", ScreenGraph::ScreenTag::SystemInfo);
        addButton(&FontAwesomeRegular24, SYMBOL_SAVE_DISK, "Validate firmware", ScreenGraph::ScreenTag::FirmwareValidation);
}


SettingsScreen::~SettingsScreen()
{
        // clean up
        while (!_buttons.empty())
        {
                ButtonInfo *button = _buttons.back();
                _buttons.pop_back();
                delete button;
        }
}


lv_color_t SettingsScreen::foregroundColor() const
{
        return lv_color_hex(0xffffff);
}


void SettingsScreen::addButton(const lv_font_t *iconFont, const char *icon, const char *text, ScreenGraph::ScreenTag destScreen)
{
        // calculate page and row
        uint8_t page = static_cast<uint8_t>(_buttons.size() / BUTTONS_PER_PAGE);
        uint8_t row = static_cast<uint8_t>(_buttons.size() % BUTTONS_PER_PAGE);

        // store the button's info
        ButtonInfo *button = new ButtonInfo();
        button->page = page;
        button->row = row;
        button->iconFont = iconFont;
        button->icon = icon;
        button->text = text;
        button->destScreen = destScreen;
        button->screen = this;
        _buttons.push_back(button);

        // update number of pages
        _pages = page + 1;
}


void SettingsScreen::handleCurrentPageChange()
{
        // create title label
        createTitleLabel("Settings");

        // create the buttons for the current page
        for (ButtonInfo *buttonInfo : _buttons)
        {
                // skip buttons that don't belong to the current page
                if ((buttonInfo->page + 1) != currentPage())
                        continue;

                // create the background rectangle
                lv_obj_t *bgRect = lv_obj_create(lv_scr_act(), nullptr);
                lv_obj_set_style_local_radius(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
                lv_obj_set_size(bgRect, 240, BUTTON_HEIGHT);
                lv_obj_align(bgRect, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, static_cast<lv_coord_t>(buttonInfo->row) * BUTTON_HEIGHT + BUTTONS_START);

                // set the background gradient
                lv_obj_set_style_local_bg_opa(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
                lv_obj_set_style_local_bg_color(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
                /*lv_obj_set_style_local_bg_grad_color(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
                lv_obj_set_style_local_bg_grad_dir(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
                lv_obj_set_style_local_bg_main_stop(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 184);
                lv_obj_set_style_local_bg_grad_stop(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);*/

                // set the border
                lv_obj_set_style_local_border_opa(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
                if (buttonInfo->row == 0)
                        lv_obj_set_style_local_border_side(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
                else
                        lv_obj_set_style_local_border_side(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
                lv_obj_set_style_local_border_color(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x808080));
                lv_obj_set_style_local_border_width(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 1);
                //lv_obj_set_style_local_border_opa(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

                // create the icon
                lv_obj_t *iconLabel = createLabel(bgRect, buttonInfo->iconFont, foregroundColor(), LV_LABEL_ALIGN_CENTER, false);
                lv_label_set_text_static(iconLabel, buttonInfo->icon);
                lv_obj_align(iconLabel, nullptr, LV_ALIGN_CENTER, -100, 0);

                // create the text
                lv_obj_t *textLabel = createLabel(bgRect, &font_dvs_ascii_16, foregroundColor(), LV_LABEL_ALIGN_LEFT, false);
                lv_label_set_text_static(textLabel, buttonInfo->text);
                lv_obj_align(textLabel, nullptr, LV_ALIGN_IN_LEFT_MID, 48, 2);

                // create the right angle
                lv_obj_t *angleLabel = createLabel(bgRect, &FontAwesomeSolid24, foregroundColor(), LV_LABEL_ALIGN_RIGHT, false);
                lv_label_set_text_static(angleLabel, SYMBOL_ANGLE_RIGHT);
                lv_obj_align(angleLabel, nullptr, LV_ALIGN_IN_RIGHT_MID, -10, 0);

                // overlay a transparent button object
                lv_obj_t *button = lv_btn_create(bgRect, nullptr);
                lv_obj_set_user_data(button, buttonInfo);
                lv_obj_set_style_local_bg_opa(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
                lv_obj_set_size(button, 240, BUTTON_HEIGHT);
                lv_obj_align(button, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
                lv_obj_set_event_cb(button, buttonCallback);
        }
}


void SettingsScreen::buttonClicked(int row)
{
        // activate the corresponding screen
        for (ButtonInfo *buttonInfo : _buttons)
        {
                if (((buttonInfo->page + 1) == currentPage()) && (buttonInfo->row == row))
                {
                        screenGraph()->activateScreen(buttonInfo->destScreen, ScreenGraph::TransitionEffect::MoveLeft);
                        break;
                }
        }
}


void SettingsScreen::buttonCallback(lv_obj_t *object, lv_event_t event)
{
        if (event == LV_EVENT_CLICKED)
        {
                ButtonInfo *buttonInfo = reinterpret_cast<ButtonInfo *>(object->user_data);
                buttonInfo->screen->buttonClicked(buttonInfo->row);
        }
}
