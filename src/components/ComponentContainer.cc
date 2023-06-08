#include "components/ComponentContainer.h"



ComponentContainer::ComponentContainer(const Battery *battery,
                                       const Ble *ble,
                                       BrightnessController *brightness,
                                       DateTime *dateTime,
                                       FirmwareValidator *firmwareValidator,
                                       MotionController *motion,
                                       Settings *settings,
                                       const Watchdog *watchdog)
{
        _battery = battery;
        _ble = ble;
        _brightness = brightness;
        _dateTime = dateTime;
        _firmwareValidator = firmwareValidator;
        _motion = motion;
        _settings = settings;
        _watchdog = watchdog;
}
