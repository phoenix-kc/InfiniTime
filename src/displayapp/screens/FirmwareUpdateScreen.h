#ifndef FIRMWAREUPDATESCREEN_H
#define FIRMWAREUPDATESCREEN_H


#include "Screen.h"



class FirmwareUpdateScreen : public Screen
{
public:

        FirmwareUpdateScreen(ScreenGraph *screenGraph, ComponentContainer *components);

        void refresh() override;

private:

        lv_obj_t *_titleLabel;
        lv_obj_t *_progressBar;
        lv_obj_t *_percentageLabel;

        uint32_t _ticksAtStart;
        Pinetime::Controllers::Ble::FirmwareUpdateStates _state;

        void displayProgress();
        void updateValidated();
        void updateError();
};

#endif // FIRMWAREUPDATESCREEN_H
