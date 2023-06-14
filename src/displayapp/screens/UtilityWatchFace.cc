#include "UtilityWatchFace.h"

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

char const* WeekDays[] = {"--", "MO", "TU", "WE", "TH", "FR", "SA", "SU"};


LV_IMG_DECLARE(image_utility_watchface_bg);



UtilityWatchFace::UtilityWatchFace(ScreenGraph *screenGraph, ComponentContainer *components)
        : WatchFace(screenGraph, components)
{
        // create the basic style for arcs (background, padding, etc.)
        lv_style_init(&_basicArcStyle);
        lv_style_set_line_width(&_basicArcStyle, LV_STATE_DEFAULT, 8);
        lv_style_set_line_rounded(&_basicArcStyle, LV_STATE_DEFAULT, true);
        lv_style_set_bg_opa(&_basicArcStyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
        lv_style_set_pad_all(&_basicArcStyle, LV_STATE_DEFAULT, 0);
        lv_style_set_border_opa(&_basicArcStyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);

        // set the watch's background image
        lv_obj_t* backgroundImage = lv_img_create(lv_scr_act(), nullptr);
        lv_img_set_src(backgroundImage, &image_utility_watchface_bg);
        lv_obj_align(backgroundImage, nullptr, LV_ALIGN_CENTER, 0, 0);

        // add the day-of-week label
        _dayOfWeekLabel = createLabel(&font_dvs_ascii_16, lv_color_hex(0xffffff), LV_LABEL_ALIGN_CENTER, true);
        lv_label_set_text_static(_dayOfWeekLabel, dayOfWeekText());
        lv_obj_align(_dayOfWeekLabel, nullptr, LV_ALIGN_CENTER, 32, 0);

        // add the day-of-month label
        _dayOfMonthLabel = createLabel(&font_dvs_ascii_16, lv_color_hex(0xfe3b30), LV_LABEL_ALIGN_CENTER, true);
        lv_label_set_text_fmt(_dayOfMonthLabel, "%d", static_cast<int>(dayOfMonth()));
        lv_obj_align(_dayOfMonthLabel, nullptr, LV_ALIGN_CENTER, 64, 0);

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


void UtilityWatchFace::dayChanged(bool yearChanged, bool monthChanged)
{
        (void)yearChanged;
        (void)monthChanged;

        // update day-of-week label
        lv_label_set_text_static(_dayOfWeekLabel, dayOfWeekText());

        // update day-of-month label
        lv_label_set_text_fmt(_dayOfMonthLabel, "%d", static_cast<int>(dayOfMonth()));
}


void UtilityWatchFace::timeChanged(bool hourChanged, bool minuteChanged)
{
        // update the hands
        updateHands(hourChanged, minuteChanged);
}


void UtilityWatchFace::lockedStateChanged()
{
        lv_label_set_text_static(_lockSymbol, locked() ? SYMBOL_LOCK : "");
        lv_obj_align(_lockSymbol, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
}


void UtilityWatchFace::batteryPercentChanged()
{
        int angle = static_cast<int>(static_cast<double>(batteryPercent()) / 100.0 * 30.0 + 0.5);
        if (angle > 30)
                angle = 30;
        else if (angle < 0)
                angle = 0;
        lv_arc_set_angles(_batteryArc, static_cast<uint16_t>(60 - angle), 60);
        lv_label_set_text_fmt(_batteryLabel, "%%\n%d", batteryPercent());
}


void UtilityWatchFace::updateHands(bool hourChanged, bool minuteChanged)
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
        _minuteHandOuterPoints[1].x = cx + roundedCoord(100.0 * sine);
        _minuteHandOuterPoints[1].y = cy - roundedCoord(100.0 * cosine);
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


void UtilityWatchFace::updatePowerAndBleSymbols()
{
        lv_label_set_text_static(_powerPresentSymbol, powerPresent() ? SYMBOL_PLUG : "");
        lv_label_set_text_static(_bleSymbol, bleConnected() ? SYMBOL_BLUETOOTH : "");
        lv_obj_align(_powerPresentSymbol, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
        if (powerPresent())
                lv_obj_align(_bleSymbol, _powerPresentSymbol, LV_ALIGN_OUT_LEFT_MID, -4, 0);
        else
                lv_obj_align(_bleSymbol, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
}


int16_t UtilityWatchFace::roundedCoord(double value)
{
        if (value < 0.0)
                return static_cast<int16_t>(value - 0.5);
        else
                return static_cast<int16_t>(value + 0.5);
}


const char *UtilityWatchFace::dayOfWeekText()
{
        return WeekDays[static_cast<uint8_t>(this->components()->dateTime()->DayOfWeek())];
}

