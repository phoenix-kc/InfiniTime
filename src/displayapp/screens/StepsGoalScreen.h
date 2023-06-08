#ifndef STEPSGOALSCREEN_H
#define STEPSGOALSCREEN_H


#include "Screen.h"



class StepsGoalScreen : public Screen
{
public:

        StepsGoalScreen(ScreenGraph *screenGraph, ComponentContainer *components);

private:

        lv_obj_t *_iconLabel;
        lv_obj_t *_stepsLabel;

        void updateStepsLabel();

        static void buttonCallback(int increment, lv_obj_t *object, lv_event_t event);
        static void buttonInc100Callback(lv_obj_t *object, lv_event_t event) { buttonCallback(100, object, event); }
        static void buttonInc1000Callback(lv_obj_t *object, lv_event_t event) { buttonCallback(1000, object, event); }
        static void buttonDec100Callback(lv_obj_t *object, lv_event_t event) { buttonCallback(-100, object, event); }
        static void buttonDec1000Callback(lv_obj_t *object, lv_event_t event) { buttonCallback(-1000, object, event); }
};

#endif // STEPSGOALSCREEN_H
