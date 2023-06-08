#include "WatchFace.h"

#include <lvgl/lvgl.h>


using namespace Pinetime::Controllers;



WatchFace::WatchFace(ScreenGraph *screenGraph, ComponentContainer *components)
        : Screen(screenGraph, components)
{
        // initialize
        DateTime *dateTime = this->components()->dateTime();
        _year = dateTime->Year();
        _month = dateTime->Month();
        _dayOfWeek = dateTime->DayOfWeek();
        _dayOfMonth = dateTime->Day();
        _hour = dateTime->Hours();
        _minute = dateTime->Minutes();
        _second = dateTime->Seconds();

        _locked = false;
        _batteryPercent = this->components()->battery()->PercentRemaining();
        _powerPresent = this->components()->battery()->IsPowerPresent();
        _bleConnected = this->components()->ble()->IsConnected();

        _firstRefresh = true;
}


void WatchFace::refresh()
{
        // store the current watch face as default on the first call
        if (_firstRefresh)
        {
                screenGraph()->setDefaultWatchFace(screenTag());
                _firstRefresh = false;
        }

        // get current date and time
        DateTime *dateTime = this->components()->dateTime();
        uint16_t year = dateTime->Year();
        DateTime::Months month = dateTime->Month();
        DateTime::Days dayOfWeek = dateTime->DayOfWeek();
        uint8_t dayOfMonth = dateTime->Day();
        uint8_t hour = dateTime->Hours();
        uint8_t minute = dateTime->Minutes();
        uint8_t second = dateTime->Seconds();

        // get current power / connection status
        uint8_t batteryPercent = this->components()->battery()->PercentRemaining();
        bool powerPresent = this->components()->battery()->IsPowerPresent();
        bool bleConnected = this->components()->ble()->IsConnected();

        // find out if something's changed
        bool yearHasChanged = (year != _year);
        bool monthHasChanged = (month != _month);
        bool dayHasChanged = ((dayOfMonth != _dayOfMonth) || monthHasChanged || yearHasChanged);
        bool hourHasChanged = (hour != _hour);
        bool minuteHasChanged = (minute != _minute);
        bool timeHasChanged = (hourHasChanged || minuteHasChanged || (second != _second));
        bool batteryPercentHasChanged = (batteryPercent != _batteryPercent);
        bool powerPresentHasChanged = (powerPresent != _powerPresent);
        bool bleConnectedHasChanged = (bleConnected != _bleConnected);

        // update the stored values
        _year = year;
        _month = month;
        _dayOfWeek = dayOfWeek;
        _dayOfMonth = dayOfMonth;
        _hour = hour;
        _minute = minute;
        _second = second;
        _batteryPercent = batteryPercent;
        _powerPresent = powerPresent;
        _bleConnected = bleConnected;

        // call the handlers
        if (dayHasChanged)
                dayChanged(yearHasChanged, monthHasChanged);
        if (timeHasChanged)
                timeChanged(hourHasChanged, minuteHasChanged);
        if (batteryPercentHasChanged)
                batteryPercentChanged();
        if (powerPresentHasChanged)
                powerPresentChanged();
        if (bleConnectedHasChanged)
                bleConnectedChanged();
}


bool WatchFace::handleButtonPress()
{
        _locked = !_locked;
        lockedStateChanged();
        return true;
}


bool WatchFace::handleTap()
{
        // ignore the event if the screen is locked
        return _locked;
}


bool WatchFace::handleLongTap()
{
        // ignore the event if the screen is locked
        return _locked;
}


bool WatchFace::handleDoubleTap()
{
        // ignore the event if the screen is locked
        return _locked;
}


bool WatchFace::handleSwipe(SwipeDirection direction)
{
        // ignore the event if the screen is locked
        (void)direction;
        return _locked;
}
