#ifndef WATCHFACE_H
#define WATCHFACE_H


#include "Screen.h"
#include "ScreenGraph.h"


using namespace Pinetime::Controllers;



class WatchFace : public Screen
{
public:

        WatchFace(ScreenGraph *screenGraph, ComponentContainer *components);

        void refresh() override;

        uint16_t year() const { return _year; }
        DateTime::Months month() const { return _month; }
        DateTime::Days dayOfWeek() const { return _dayOfWeek; }
        uint8_t dayOfMonth() const { return _dayOfMonth; }
        uint8_t hour() const { return _hour; }
        uint8_t minute() const { return _minute; }
        uint8_t second() const { return _second; }

        uint8_t batteryPercent() const { return _batteryPercent; }
        bool powerPresent() const { return _powerPresent; }
        bool bleConnected() const { return _bleConnected; }

        bool locked() const { return _locked; }

        bool handleButtonPress() override;
        bool handleTap() override;
        bool handleLongTap() override;
        bool handleDoubleTap() override;
        bool handleSwipe(SwipeDirection direction) override;

protected:

        virtual ScreenGraph::ScreenTag screenTag() const = 0;

        virtual void timeChanged(bool hourChanged, bool minuteChanged) { (void)hourChanged; (void)minuteChanged; }
        virtual void dayChanged(bool yearChanged, bool monthChanged) { (void)yearChanged; (void)monthChanged; }

        virtual void lockedStateChanged() { }
        virtual void batteryPercentChanged() { }
        virtual void powerPresentChanged() { }
        virtual void bleConnectedChanged() { }

private:

        uint16_t _year;
        DateTime::Months _month;
        DateTime::Days _dayOfWeek;
        uint8_t _dayOfMonth;
        uint8_t _hour;
        uint8_t _minute;
        uint8_t _second;

        bool _locked;
        uint8_t _batteryPercent;
        bool _powerPresent;
        bool _bleConnected;

        bool _firstRefresh;
};

#endif // WATCHFACE_H
