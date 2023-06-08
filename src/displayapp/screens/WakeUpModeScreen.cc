#include "WakeUpModeScreen.h"

#include <lvgl/lvgl.h>

#include "displayapp/fonts/font_dvsb_ascii_18.h"
#include "displayapp/fonts/font_dvs_ascii_16.h"
#include "displayapp/fonts/FontAwesomeRegular24.h"
#include "displayapp/fonts/FontAwesomeSolid24.h"


#define OPTION_HEIGHT   52
#define OPTIONS_START   30



WakeUpModeScreen::WakeUpModeScreen(ScreenGraph *screenGraph, ComponentContainer *components)
        : Screen(screenGraph, components)
{
        // init
        _optionInfos.clear();

        // create title label
        createTitleLabel("Wake Up Mode");

        // add the options
        addOption("Single tap");
        addOption("Double tap");
        addOption("Raise wrist");

        // make sure that the current setting is checked
        updateCheckBoxes();
}


WakeUpModeScreen::~WakeUpModeScreen()
{
        // clean up
        while (!_optionInfos.empty())
        {
                OptionInfo *optionInfo = _optionInfos.back();
                _optionInfos.pop_back();
                delete optionInfo;
        }
}


void WakeUpModeScreen::addOption(const char *text)
{
        // create the background rectangle
        lv_obj_t *bgRect = lv_obj_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_radius(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_size(bgRect, 240, OPTION_HEIGHT);
        lv_obj_align(bgRect, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, static_cast<lv_coord_t>(_optionInfos.size()) * OPTION_HEIGHT + OPTIONS_START);

        // set the background gradient
        lv_obj_set_style_local_bg_opa(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_obj_set_style_local_bg_color(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, backgroundColor());
        /*lv_obj_set_style_local_bg_grad_color(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
        lv_obj_set_style_local_bg_grad_dir(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_obj_set_style_local_bg_main_stop(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 184);
        lv_obj_set_style_local_bg_grad_stop(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);*/

        // set the border
        /*lv_obj_set_style_local_border_opa(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
        if (buttonInfo->row == 0)
                lv_obj_set_style_local_border_side(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
        else
                lv_obj_set_style_local_border_side(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
        lv_obj_set_style_local_border_color(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x808080));
        lv_obj_set_style_local_border_width(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 1);*/
        lv_obj_set_style_local_border_opa(bgRect, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

        // create the checkbox icon
        lv_obj_t *checkBoxLabel = createLabel(bgRect, &FontAwesomeRegular24, lv_color_hex(0x808080), LV_LABEL_ALIGN_CENTER, true);
        lv_label_set_text_static(checkBoxLabel, SYMBOL_SQUARE);
        lv_obj_align(checkBoxLabel, nullptr, LV_ALIGN_CENTER, -104, 0);

        // create the text
        lv_obj_t *textLabel = createLabel(bgRect, &font_dvs_ascii_16, foregroundColor(), LV_LABEL_ALIGN_LEFT, false);
        lv_label_set_text_static(textLabel, text);
        lv_obj_align(textLabel, nullptr, LV_ALIGN_IN_LEFT_MID, 48, 2);

        // create the option info
        OptionInfo *optionInfo = new OptionInfo();
        optionInfo->screen = this;
        optionInfo->row = static_cast<uint8_t>(_optionInfos.size());
        optionInfo->checkBoxLabel = checkBoxLabel;
        _optionInfos.push_back(optionInfo);

        // overlay a transparent button object
        lv_obj_t *button = lv_btn_create(bgRect, nullptr);
        lv_obj_set_user_data(button, optionInfo);
        lv_obj_set_style_local_bg_opa(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
        lv_obj_set_size(button, 240, OPTION_HEIGHT);
        lv_obj_align(button, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
        lv_obj_set_event_cb(button, buttonCallback);
}


void WakeUpModeScreen::updateCheckBoxes()
{
        for (OptionInfo *optionInfo : _optionInfos)
        {
                bool checked = false;
                switch (optionInfo->row)
                {
                case 0:   // single tap
                        checked = components()->settings()->isWakeUpModeOn(Settings::WakeUpMode::SingleTap);
                        break;
                case 1:   // double tap
                        checked = components()->settings()->isWakeUpModeOn(Settings::WakeUpMode::DoubleTap);
                        break;
                case 2:   // raise wrist
                        checked = components()->settings()->isWakeUpModeOn(Settings::WakeUpMode::RaiseWrist);
                        break;
                default:
                        break;
                }

                if (checked)
                {
                        lv_label_set_text_static(optionInfo->checkBoxLabel, SYMBOL_CHECK_SQUARE);
                        lv_obj_set_style_local_text_color(optionInfo->checkBoxLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, foregroundColor());
                }
                else
                {
                        lv_label_set_text_static(optionInfo->checkBoxLabel, SYMBOL_SQUARE);
                        lv_obj_set_style_local_text_color(optionInfo->checkBoxLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x808080));
                }
        }
}


void WakeUpModeScreen::optionClicked(int row)
{
        // toggle selected mode
        Settings::WakeUpMode mode;
        switch (row)
        {
        case 1:    // double tap
                mode = Settings::WakeUpMode::DoubleTap;
                break;
        case 2:    // raise wrist
                mode = Settings::WakeUpMode::RaiseWrist;
                break;
        default:   // single tap
                mode = Settings::WakeUpMode::SingleTap;
                break;
        }
        bool previousMode = components()->settings()->isWakeUpModeOn(mode);
        components()->settings()->setWakeUpMode(mode, !previousMode);

        // make sure that the checkboxes show the new setting
        updateCheckBoxes();
}


void WakeUpModeScreen::buttonCallback(lv_obj_t *object, lv_event_t event)
{
        if (event == LV_EVENT_CLICKED)
        {
                OptionInfo *optionInfo = reinterpret_cast<OptionInfo *>(object->user_data);
                optionInfo->screen->optionClicked(optionInfo->row);
        }
}
