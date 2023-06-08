#ifndef BINARYWATCHFACE_H
#define BINARYWATCHFACE_H


#include "WatchFace.h"



class BinaryWatchFace : public WatchFace
{
public:

        BinaryWatchFace(ScreenGraph *screenGraph, ComponentContainer *components);

protected:

        ScreenGraph::ScreenTag screenTag() const override { return ScreenGraph::ScreenTag::BinaryWatchFace; }

        //void dayChanged(bool yearChanged, bool monthChanged) override;
        void timeChanged(bool hourChanged, bool minuteChanged) override;

        void lockedStateChanged() override;
        //void batteryPercentChanged() override;
        void powerPresentChanged() override { updatePowerAndBleSymbols(); }
        void bleConnectedChanged() override { updatePowerAndBleSymbols(); }

private:

        lv_style_t _lineStyle;
        lv_obj_t *_hourLines[4];
        lv_point_t _hourLinePoints[8];
        lv_obj_t *_minuteLines[6];
        lv_point_t _minuteLinePoints[12];
        lv_obj_t *_secondLines[6];
        lv_point_t _secondLinePoints[12];
        lv_obj_t *_powerPresentSymbol;
        lv_obj_t *_bleSymbol;
        lv_obj_t *_lockSymbol;

        void updatePowerAndBleSymbols();
};

#endif // BINARYWATCHFACE_H
