#include "StepsGoalScreen.h"

#include <lvgl/lvgl.h>

#include "displayapp/fonts/font_dvsb_ascii_18.h"
#include "displayapp/fonts/FontAwesomeSolid24.h"


#define MIN_STEPS_GOAL   500
#define MAX_STEPS_GOAL   50000

#define BUTTON_WIDTH     80
#define BUTTON_HEIGHT    58
#define BUTTON_DX        54
#define BUTTON_DY        60
#define MIDDLE_OFFSET    16



StepsGoalScreen::StepsGoalScreen(ScreenGraph *screenGraph, ComponentContainer *components)
        : Screen(screenGraph, components)
{
        // create title label
        createTitleLabel("Activity Goal");

        // create the "walking man" icon
        _iconLabel = createLabel(&FontAwesomeSolid24, lv_color_hex(0xa6fe00), LV_LABEL_ALIGN_CENTER, false);
        lv_label_set_text_static(_iconLabel, SYMBOL_WALKING);

        // create the steps label
        _stepsLabel = createLabel(&font_dvsb_ascii_18, foregroundColor(), LV_LABEL_ALIGN_CENTER, true);
        lv_obj_align(_stepsLabel, nullptr, LV_ALIGN_CENTER, 16, MIDDLE_OFFSET);
        updateStepsLabel();

        // create the +1000 button
        lv_obj_t *inc1000Button = createButton(&FontAwesomeSolid24, SYMBOL_ANGLE_DOUBLE_UP, BUTTON_WIDTH, BUTTON_HEIGHT);
        lv_obj_set_user_data(inc1000Button, this);
        lv_obj_align(inc1000Button, nullptr, LV_ALIGN_CENTER, -BUTTON_DX, -BUTTON_DY + MIDDLE_OFFSET);
        lv_obj_set_event_cb(inc1000Button, buttonInc1000Callback);

        // create the +100 button
        lv_obj_t *inc100Button = createButton(&FontAwesomeSolid24, SYMBOL_ANGLE_UP, BUTTON_WIDTH, BUTTON_HEIGHT);
        lv_obj_set_user_data(inc100Button, this);
        lv_obj_align(inc100Button, nullptr, LV_ALIGN_CENTER, BUTTON_DX, -BUTTON_DY + MIDDLE_OFFSET);
        lv_obj_set_event_cb(inc100Button, buttonInc100Callback);

        // create the -1000 button
        lv_obj_t *dec1000Button = createButton(&FontAwesomeSolid24, SYMBOL_ANGLE_DOUBLE_DOWN, BUTTON_WIDTH, BUTTON_HEIGHT);
        lv_obj_set_user_data(dec1000Button, this);
        lv_obj_align(dec1000Button, nullptr, LV_ALIGN_CENTER, -BUTTON_DX, BUTTON_DY + MIDDLE_OFFSET);
        lv_obj_set_event_cb(dec1000Button, buttonDec1000Callback);

        // create the -100 button
        lv_obj_t *dec100Button = createButton(&FontAwesomeSolid24, SYMBOL_ANGLE_DOWN, BUTTON_WIDTH, BUTTON_HEIGHT);
        lv_obj_set_user_data(dec100Button, this);
        lv_obj_align(dec100Button, nullptr, LV_ALIGN_CENTER, BUTTON_DX, BUTTON_DY + MIDDLE_OFFSET);
        lv_obj_set_event_cb(dec100Button, buttonDec100Callback);
}


void StepsGoalScreen::updateStepsLabel()
{
        lv_label_set_text_fmt(_stepsLabel, "%d steps", components()->settings()->GetStepsGoal());
        lv_obj_align(_iconLabel, _stepsLabel, LV_ALIGN_OUT_LEFT_MID, -16, 0);
}


void StepsGoalScreen::buttonCallback(int increment, lv_obj_t *object, lv_event_t event)
{
        if (event == LV_EVENT_CLICKED)
        {
                StepsGoalScreen *screen = reinterpret_cast<StepsGoalScreen *>(object->user_data);
                int newSteps = static_cast<int>(screen->components()->settings()->GetStepsGoal()) + increment;
                if (newSteps < MIN_STEPS_GOAL)
                        newSteps = MIN_STEPS_GOAL;
                if (newSteps > MAX_STEPS_GOAL)
                        newSteps = MAX_STEPS_GOAL;
                screen->components()->settings()->SetStepsGoal(static_cast<uint32_t>(newSteps));
                screen->updateStepsLabel();
        }
}
