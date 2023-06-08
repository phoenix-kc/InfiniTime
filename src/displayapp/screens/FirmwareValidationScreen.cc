#include "FirmwareValidationScreen.h"

#include <lvgl/lvgl.h>

#include "displayapp/fonts/font_dvsb_ascii_18.h"
#include "displayapp/fonts/font_dvs_ascii_16.h"



FirmwareValidationScreen::FirmwareValidationScreen(ScreenGraph *screenGraph, ComponentContainer *components)
        : Screen(screenGraph, components)
{
        // add title label
        createTitleLabel("Firmware Validation");

        // the remaining widgets depend on whether the firmware has already been validated
        if (this->components()->firmwareValidator()->IsValidated())
        {
                // create the description
                _descriptionLabel = createLabel(&font_dvs_ascii_16, lv_color_hex(0x808080), LV_LABEL_ALIGN_CENTER, false);
                lv_label_set_text_static(_descriptionLabel,
                                         "The firmware has already\n"
                                         "been validated.\n\n"
                                         "Do you want to undo the\n"
                                         "validation?");
                lv_obj_align(_descriptionLabel, nullptr, LV_ALIGN_IN_TOP_MID, 0, 52);

                // create the button
                _button = lv_btn_create(lv_scr_act(), nullptr);
                lv_obj_set_style_local_bg_color(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xa00000));
                lv_obj_set_style_local_bg_opa(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
                lv_obj_set_style_local_radius(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 24);
                lv_obj_set_style_local_border_color(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
                lv_obj_set_style_local_border_opa(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
                lv_obj_set_style_local_border_width(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 2);
                lv_obj_set_size(_button, 200, 64);
                lv_obj_align(_button, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, -16);
                lv_obj_set_user_data(_button, this);
                lv_obj_set_event_cb(_button, buttonEventCallback);

                // create the button's label
                lv_obj_t *buttonLabel = createLabel(_button, &font_dvsb_ascii_18, lv_color_hex(0xffffff), LV_LABEL_ALIGN_CENTER, false);
                lv_label_set_text_static(buttonLabel, "Undo");
        }
        else
        {
                // create the description
                _descriptionLabel = createLabel(&font_dvs_ascii_16, lv_color_hex(0x808080), LV_LABEL_ALIGN_CENTER, false);
                lv_label_set_text_static(_descriptionLabel,
                                         "If the firmware works\n"
                                         "as expected, you can\n"
                                         "validate it so that it\n"
                                         "will be loaded each\n"
                                         "time the device starts.");
                lv_obj_align(_descriptionLabel, nullptr, LV_ALIGN_IN_TOP_MID, 0, 52);

                // create the button
                _button = lv_btn_create(lv_scr_act(), nullptr);
                lv_obj_set_style_local_bg_color(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x008000));
                lv_obj_set_style_local_bg_opa(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
                lv_obj_set_style_local_radius(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 24);
                lv_obj_set_style_local_border_color(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
                lv_obj_set_style_local_border_opa(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
                lv_obj_set_style_local_border_width(_button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 2);
                lv_obj_set_size(_button, 200, 64);
                lv_obj_align(_button, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, -16);
                lv_obj_set_user_data(_button, this);
                lv_obj_set_event_cb(_button, buttonEventCallback);

                // create the button's label
                lv_obj_t *buttonLabel = createLabel(_button, &font_dvsb_ascii_18, lv_color_hex(0xffffff), LV_LABEL_ALIGN_CENTER, false);
                lv_label_set_text_static(buttonLabel, "Validate");
        }
}


void FirmwareValidationScreen::handleButtonClick()
{
        // the button is no longer required
        lv_obj_del(_button);
        _button = nullptr;

        // validate or reset?
        if (components()->firmwareValidator()->IsValidated())
        {
                // reset
                components()->firmwareValidator()->Reset();

                // change the description
                lv_label_set_text_static(_descriptionLabel,
                                         "The firmware is no\n"
                                         "longer validated.\n\n"
                                         "When the device restarts\n"
                                         "the previous firmware\n"
                                         "will be loaded.");
                lv_obj_set_style_local_text_color(_descriptionLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xff0000));
                lv_obj_align(_descriptionLabel, nullptr, LV_ALIGN_CENTER, 0, 0);
        }
        else
        {
                // validate
                components()->firmwareValidator()->Validate();

                // change the description
                lv_label_set_text_static(_descriptionLabel,
                                         "The firmware has been\n"
                                         "validated and will be\n"
                                         "loaded whenever the\n"
                                         "device restarts.");
                lv_obj_set_style_local_text_color(_descriptionLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00a000));
                lv_obj_align(_descriptionLabel, nullptr, LV_ALIGN_CENTER, 0, 0);
        }
}


void FirmwareValidationScreen::buttonEventCallback(lv_obj_t *object, lv_event_t event)
{
        if (event == LV_EVENT_CLICKED)
        {
                FirmwareValidationScreen *screen = reinterpret_cast<FirmwareValidationScreen *>(object->user_data);
                screen->handleButtonClick();
        }
}
