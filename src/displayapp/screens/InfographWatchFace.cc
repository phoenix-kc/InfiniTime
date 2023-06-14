#include "InfographWatchFace.h"

#include <cmath>
#include <lvgl/lvgl.h>

#include "displayapp/fonts/font_symbols_32.h"
#include "displayapp/fonts/FontAwesomeBrands24.h"
#include "displayapp/fonts/FontAwesomeSolid24.h"
#include "displayapp/fonts/font_dvs_digits_24.h"
#include "displayapp/fonts/font_dvs_digits_20.h"
#include "displayapp/fonts/font_dvs_ascii_14.h"
#include "displayapp/fonts/font_dvs_ascii_16.h"
#include "components/ComponentContainer.h"


using namespace Pinetime::Controllers;


#ifndef PI
#define PI   3.14159265358979323846
#endif

#define SYMBOL_DEGREES   "\xC2\xB0"


LV_IMG_DECLARE(image_infograph_watchface_bg);



InfographWatchFace::InfographWatchFace(ScreenGraph *screenGraph, ComponentContainer *components)
        : WatchFace(screenGraph, components)
{
        // initialize
        _prevSteps = this->components()->motion()->NbSteps();
        _prevStepsGoal = this->components()->settings()->GetStepsGoal();

        // create the basic style for arcs (background, padding, etc.)
        lv_style_init(&_basicArcStyle);
        lv_style_set_line_width(&_basicArcStyle, LV_STATE_DEFAULT, 8);
        lv_style_set_line_rounded(&_basicArcStyle, LV_STATE_DEFAULT, true);
        lv_style_set_bg_opa(&_basicArcStyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
        lv_style_set_pad_all(&_basicArcStyle, LV_STATE_DEFAULT, 0);
        lv_style_set_border_opa(&_basicArcStyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);

        // set the watch's background image
        lv_obj_t* backgroundImage = lv_img_create(lv_scr_act(), nullptr);
        lv_img_set_src(backgroundImage, &image_infograph_watchface_bg);
        lv_obj_align(backgroundImage, nullptr, LV_ALIGN_CENTER, 0, 0);

        // add the day-of-week label
        _dayOfWeekLabel = createLabel(&font_dvs_ascii_16, lv_color_hex(0xfe3b30), LV_LABEL_ALIGN_CENTER, true);
        lv_label_set_text_static(_dayOfWeekLabel, this->components()->dateTime()->DayOfWeekShortToString());
        lv_obj_align(_dayOfWeekLabel, nullptr, LV_ALIGN_CENTER, 0, -72);

        // add the day-of-month label
        _dayOfMonthLabel = createLabel(&font_dvs_digits_24, lv_color_hex(0xffffff), LV_LABEL_ALIGN_CENTER, true);
        lv_label_set_text_fmt(_dayOfMonthLabel, "%d", static_cast<int>(dayOfMonth()));
        lv_obj_align(_dayOfMonthLabel, nullptr, LV_ALIGN_CENTER, 0, -50);

        // add the weather symbol
        _weatherSymbol = createLabel(&font_symbols_32, lv_color_hex(0xffffff), LV_LABEL_ALIGN_CENTER, true);
        lv_label_set_text_static(_weatherSymbol, SYMBOL_SUN);
        lv_obj_align(_weatherSymbol, nullptr, LV_ALIGN_CENTER, -60, 0);

        // add the activity symbol (walking guy)
        lv_obj_t *activitySymbol = createLabel(&font_symbols_32, lv_color_hex(0xa6fe00), LV_LABEL_ALIGN_CENTER, false);
        lv_label_set_text_static(activitySymbol, SYMBOL_WALKING);
        lv_obj_align(activitySymbol, nullptr, LV_ALIGN_CENTER, 0, 60);

        // temperature dummy
//        lv_obj_t *tempDummy = createLabel(&font_dvs_digits_20, lv_color_hex(0xffffff), LV_LABEL_ALIGN_CENTER, true);
//        lv_label_set_text_fmt(tempDummy, "-12%s", SYMBOL_DEGREES);
//        lv_obj_align(tempDummy, nullptr, LV_ALIGN_CENTER, 60, 0);

        // add the steps arc
        _stepsArc = lv_arc_create(lv_scr_act(), nullptr);
        lv_arc_set_adjustable(_stepsArc, false);
        lv_obj_add_style(_stepsArc, LV_ARC_PART_BG, &_basicArcStyle);
        lv_obj_add_style(_stepsArc, LV_ARC_PART_INDIC, &_basicArcStyle);
        lv_obj_set_style_local_line_color(_stepsArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x213300));
        lv_obj_set_style_local_line_color(_stepsArc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0xa6fe00));
        lv_arc_set_bg_angles(_stepsArc, 0, 360);
        lv_arc_set_angles(_stepsArc, 270, stepsEndAngle(_prevSteps));
        lv_obj_set_size(_stepsArc, 64, 64);
        lv_obj_align(_stepsArc, nullptr, LV_ALIGN_CENTER, 0, 60);

        // add the battery arc
        _batteryArc = lv_arc_create(lv_scr_act(), nullptr);
        lv_arc_set_adjustable(_batteryArc, false);
        lv_obj_add_style(_batteryArc, LV_ARC_PART_BG, &_basicArcStyle);
        lv_obj_add_style(_batteryArc, LV_ARC_PART_INDIC, &_basicArcStyle);
        lv_obj_set_style_local_line_color(_batteryArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x014d27));
        lv_obj_set_style_local_line_color(_batteryArc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x04de71));
        lv_arc_set_bg_angles(_batteryArc, 30, 60);
        lv_arc_set_angles(_batteryArc, 30, 60);
        lv_obj_set_size(_batteryArc, 272, 272);
        lv_obj_align(_batteryArc, nullptr, LV_ALIGN_CENTER, 0, 0);

        // add the battery label
        _batteryLabel = createLabel(&font_dvs_ascii_14, lv_color_hex(0x04de71), LV_LABEL_ALIGN_RIGHT, true);
        lv_label_set_text_fmt(_batteryLabel, "%%\n%d", batteryPercent());
        lv_obj_align(_batteryLabel, nullptr, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

        // add the hour hand
        _hourHandInner = lv_line_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_line_width(_hourHandInner, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 5);
        lv_obj_set_style_local_line_color(_hourHandInner, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
        lv_obj_set_style_local_line_rounded(_hourHandInner, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, false);
        _hourHandOuter = lv_line_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_line_width(_hourHandOuter, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 11);
        lv_obj_set_style_local_line_color(_hourHandOuter, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
        lv_obj_set_style_local_line_rounded(_hourHandOuter, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, true);

        // add the minute hand
        _minuteHandInner = lv_line_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_line_width(_minuteHandInner, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 5);
        lv_obj_set_style_local_line_color(_minuteHandInner, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
        lv_obj_set_style_local_line_rounded(_minuteHandInner, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, false);
        _minuteHandOuter = lv_line_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_line_width(_minuteHandOuter, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 11);
        lv_obj_set_style_local_line_color(_minuteHandOuter, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
        lv_obj_set_style_local_line_rounded(_minuteHandOuter, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, true);

        // add the second hand
        _secondHand = lv_line_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_line_width(_secondHand, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 3);
        lv_obj_set_style_local_line_color(_secondHand, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xfe3b30));
        lv_obj_set_style_local_line_rounded(_secondHand, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, true);
        _secondTail = lv_line_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_line_width(_secondTail, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 3);
        lv_obj_set_style_local_line_color(_secondTail, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xfe3b30));
        lv_obj_set_style_local_line_rounded(_secondTail, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, true);

        // add the charging and BLE symbols
        _powerPresentSymbol = createLabel(&FontAwesomeSolid24, lv_color_hex(0x04de71), LV_LABEL_ALIGN_CENTER, false);
        _bleSymbol = createLabel(&FontAwesomeBrands24, lv_color_hex(0x2020ff), LV_LABEL_ALIGN_CENTER, false);

        // add the lock symbol
        _lockSymbol = createLabel(&FontAwesomeSolid24, lv_color_hex(0x404040), LV_LABEL_ALIGN_CENTER, false);

        // update everything
        updateHands(true, true);
        batteryPercentChanged();
        updatePowerAndBleSymbols();
        lockedStateChanged();
}


void InfographWatchFace::dayChanged(bool yearChanged, bool monthChanged)
{
        (void)yearChanged;
        (void)monthChanged;

        // update day-of-week label
        lv_label_set_text_static(_dayOfWeekLabel, this->components()->dateTime()->DayOfWeekShortToString());

        // update day-of-month label
        lv_label_set_text_fmt(_dayOfMonthLabel, "%d", static_cast<int>(dayOfMonth()));
}


void InfographWatchFace::timeChanged(bool hourChanged, bool minuteChanged)
{
        // update the hands
        updateHands(hourChanged, minuteChanged);

        // update steps arc
        uint32_t steps = this->components()->motion()->NbSteps();
        uint32_t stepsGoal = this->components()->settings()->GetStepsGoal();
        if ((steps != _prevSteps) || (stepsGoal != _prevStepsGoal))
                lv_arc_set_angles(_stepsArc, 270, stepsEndAngle(steps));
        _prevSteps = steps;
        _prevStepsGoal = stepsGoal;
}


void InfographWatchFace::lockedStateChanged()
{
        lv_label_set_text_static(_lockSymbol, locked() ? SYMBOL_LOCK : "");
        lv_obj_align(_lockSymbol, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
}


void InfographWatchFace::batteryPercentChanged()
{
        int angle = static_cast<int>(static_cast<double>(batteryPercent()) / 100.0 * 30.0 + 0.5);
        if (angle > 30)
                angle = 30;
        else if (angle < 0)
                angle = 0;
        lv_arc_set_angles(_batteryArc, static_cast<uint16_t>(60 - angle), 60);
        lv_label_set_text_fmt(_batteryLabel, "%%\n%d", batteryPercent());
}


void InfographWatchFace::updateHands(bool hourChanged, bool minuteChanged)
{
        int16_t cx = LV_HOR_RES / 2;
        int16_t cy = LV_VER_RES / 2;

        // update the hour hand
        if (hourChanged || minuteChanged)
        {
                double radians = ((static_cast<double>(hour() % 12) * 60.0) + static_cast<double>(minute())) / (12.0 * 60.0) * 2.0 * PI;
                double sine = sin(radians);
                double cosine = cos(radians);

                _hourHandInnerPoints[0].x = cx + roundedCoord(5.0 * sine);
                _hourHandInnerPoints[0].y = cy - roundedCoord(5.0 * cosine);
                _hourHandInnerPoints[1].x = cx + roundedCoord(20.0 * sine);
                _hourHandInnerPoints[1].y = cy - roundedCoord(20.0 * cosine);
                lv_line_set_points(_hourHandInner, _hourHandInnerPoints, 2);

                _hourHandOuterPoints[0].x = cx + roundedCoord(20.0 * sine);
                _hourHandOuterPoints[0].y = cy - roundedCoord(20.0 * cosine);
                _hourHandOuterPoints[1].x = cx + roundedCoord(60.0 * sine);
                _hourHandOuterPoints[1].y = cy - roundedCoord(60.0 * cosine);
                lv_line_set_points(_hourHandOuter, _hourHandOuterPoints, 2);
        }

        // update the minute hand
        double radians = ((static_cast<double>(minute()) * 60.0) + static_cast<double>(second())) / (60.0 * 60.0) * 2.0 * PI;
        double sine = sin(radians);
        double cosine = cos(radians);

        _minuteHandInnerPoints[0].x = cx + roundedCoord(5.0 * sine);
        _minuteHandInnerPoints[0].y = cy - roundedCoord(5.0 * cosine);
        _minuteHandInnerPoints[1].x = cx + roundedCoord(20.0 * sine);
        _minuteHandInnerPoints[1].y = cy - roundedCoord(20.0 * cosine);
        lv_line_set_points(_minuteHandInner, _minuteHandInnerPoints, 2);

        _minuteHandOuterPoints[0].x = cx + roundedCoord(20.0 * sine);
        _minuteHandOuterPoints[0].y = cy - roundedCoord(20.0 * cosine);
        _minuteHandOuterPoints[1].x = cx + roundedCoord(104.0 * sine);
        _minuteHandOuterPoints[1].y = cy - roundedCoord(104.0 * cosine);
        lv_line_set_points(_minuteHandOuter, _minuteHandOuterPoints, 2);

        // update the second hand
        radians = static_cast<double>(second()) / 60.0 * 2.0 * PI;
        sine = sin(radians);
        cosine = cos(radians);

        _secondHandPoints[0].x = cx + roundedCoord(4.0 * sine);
        _secondHandPoints[0].y = cy - roundedCoord(4.0 * cosine);
        _secondHandPoints[1].x = cx + roundedCoord(114.0 * sine);
        _secondHandPoints[1].y = cy - roundedCoord(114.0 * cosine);
        lv_line_set_points(_secondHand, _secondHandPoints, 2);

        _secondTailPoints[0].x = cx - roundedCoord(4.0 * sine);
        _secondTailPoints[0].y = cy + roundedCoord(4.0 * cosine);
        _secondTailPoints[1].x = cx - roundedCoord(20.0 * sine);
        _secondTailPoints[1].y = cy + roundedCoord(20.0 * cosine);
        lv_line_set_points(_secondTail, _secondTailPoints, 2);
}


void InfographWatchFace::updatePowerAndBleSymbols()
{
        lv_label_set_text_static(_powerPresentSymbol, powerPresent() ? SYMBOL_PLUG : "");
        lv_label_set_text_static(_bleSymbol, bleConnected() ? SYMBOL_BLUETOOTH : "");
        lv_obj_align(_powerPresentSymbol, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
        if (powerPresent())
                lv_obj_align(_bleSymbol, _powerPresentSymbol, LV_ALIGN_OUT_LEFT_MID, -4, 0);
        else
                lv_obj_align(_bleSymbol, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
}


int16_t InfographWatchFace::roundedCoord(double value)
{
        if (value < 0.0)
                return static_cast<int16_t>(value - 0.5);
        else
                return static_cast<int16_t>(value + 0.5);
}

uint16_t InfographWatchFace::stepsEndAngle(uint32_t steps)
{
        double s = static_cast<double>(steps);
        double g = static_cast<double>(this->components()->settings()->GetStepsGoal());
        double angle = ((s >= g) || (g <= 0.0)) ? 360.0 : s / g * 360.0;
        uint16_t endAngle = static_cast<uint16_t>((angle < 90.0) ? (270.0 + angle + 0.5) : (angle - 90.0 + 0.5));
        if ((endAngle == 270) && (s > (0.5 * g)))
                endAngle = 269;
        return endAngle;
}
