#ifndef FIRMWAREVALIDATIONSCREEN_H
#define FIRMWAREVALIDATIONSCREEN_H


#include "Screen.h"



class FirmwareValidationScreen : public Screen
{
public:

        FirmwareValidationScreen(ScreenGraph *screenGraph, ComponentContainer *components);

private:

        lv_obj_t *_descriptionLabel;
        lv_obj_t *_button;

        void handleButtonClick();

        static void buttonEventCallback(lv_obj_t *object, lv_event_t event);
};

#endif // FIRMWAREVALIDATIONSCREEN_H
