#include "BinaryWatchFace.h"

#include <lvgl/lvgl.h>

#include "displayapp/fonts/FontBoldDigits96.h"
#include "displayapp/fonts/font_dvs_ascii_16.h"
#include "displayapp/fonts/FontAwesomeBrands24.h"
#include "displayapp/fonts/FontAwesomeSolid24.h"


LV_IMG_DECLARE(image_binary_watchface_bg);


#define LINE_WIDTH       10
#define LINE_DX          72
#define LINE_DY          56
#define LINE_TILT        36
#define HOURS_BOTTOM     80
#define HOURS_RIGHT      192
#define MINUTES_BOTTOM   148
#define MINUTES_RIGHT    228
#define SECONDS_BOTTOM   216
#define SECONDS_RIGHT    192



BinaryWatchFace::BinaryWatchFace(ScreenGraph *screenGraph, ComponentContainer *components)
        : WatchFace(screenGraph, components)
{
        // set the watch's background image
        lv_obj_t* backgroundImage = lv_img_create(lv_scr_act(), nullptr);
        lv_img_set_src(backgroundImage, &image_binary_watchface_bg);
        lv_obj_align(backgroundImage, nullptr, LV_ALIGN_CENTER, 0, 0);

        // set up the line style
        lv_style_init(&_lineStyle);
        lv_style_set_line_width(&_lineStyle, LV_STATE_DEFAULT, LINE_WIDTH);
        lv_style_set_line_color(&_lineStyle, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
        lv_style_set_line_rounded(&_lineStyle, LV_STATE_DEFAULT, true);

        // create the hour lines
        lv_coord_t x = HOURS_RIGHT;
        for (int i = 0; i < 4; i++)
        {
                _hourLines[i] = lv_line_create(lv_scr_act(), nullptr);
                lv_obj_add_style(_hourLines[i], LV_LINE_PART_MAIN, &_lineStyle);
                _hourLinePoints[i * 2].x = x;
                _hourLinePoints[i * 2].y = HOURS_BOTTOM;
                _hourLinePoints[i * 2 + 1].x = (i & 1) ? x + LINE_TILT : x - LINE_TILT;
                _hourLinePoints[i * 2 + 1].y = HOURS_BOTTOM - LINE_DY;
                lv_line_set_points(_hourLines[i], _hourLinePoints + i * 2, 2);
                if ((i & 1) == 0)
                        x -= LINE_DX;
        }

        // create the minute lines
        x = MINUTES_RIGHT;
        for (int i = 0; i < 6; i++)
        {
                _minuteLines[i] = lv_line_create(lv_scr_act(), nullptr);
                lv_obj_add_style(_minuteLines[i], LV_LINE_PART_MAIN, &_lineStyle);
                _minuteLinePoints[i * 2].x = x;
                _minuteLinePoints[i * 2].y = MINUTES_BOTTOM;
                _minuteLinePoints[i * 2 + 1].x = (i & 1) ? x + LINE_TILT : x - LINE_TILT;
                _minuteLinePoints[i * 2 + 1].y = MINUTES_BOTTOM - LINE_DY;
                lv_line_set_points(_minuteLines[i], _minuteLinePoints + i * 2, 2);
                if ((i & 1) == 0)
                        x -= LINE_DX;
        }

        // create the second lines
        x = SECONDS_RIGHT;
        for (int i = 0; i < 6; i++)
        {
                _secondLines[i] = lv_line_create(lv_scr_act(), nullptr);
                lv_obj_add_style(_secondLines[i], LV_LINE_PART_MAIN, &_lineStyle);
                _secondLinePoints[i * 2].x = x;
                _secondLinePoints[i * 2].y = SECONDS_BOTTOM;
                _secondLinePoints[i * 2 + 1].x = (i & 1) ? x - LINE_TILT : x + LINE_TILT;
                _secondLinePoints[i * 2 + 1].y = SECONDS_BOTTOM - LINE_DY;
                lv_line_set_points(_secondLines[i], _secondLinePoints + i * 2, 2);
                if (i & 1)
                        x -= LINE_DX;
        }

        // add the charging and BLE symbols
        _powerPresentSymbol = createLabel(&FontAwesomeSolid24, lv_color_hex(0x04de71), LV_LABEL_ALIGN_CENTER, false);
        _bleSymbol = createLabel(&FontAwesomeBrands24, lv_color_hex(0x2020ff), LV_LABEL_ALIGN_CENTER, false);

        // add the lock symbol
        _lockSymbol = createLabel(&FontAwesomeSolid24, lv_color_hex(0x404040), LV_LABEL_ALIGN_CENTER, false);

        // update everything
        timeChanged(true, true);
        updatePowerAndBleSymbols();
        lockedStateChanged();
}


void BinaryWatchFace::timeChanged(bool hourChanged, bool minuteChanged)
{
        if (hourChanged)
        {
                // update the visibility of the hour lines
                int theHour = static_cast<int>(hour()) % 12;
                if (theHour == 0)
                        theHour = 12;
                for (int i = 0; i < 4; i++)
                {
                        bool visible = ((theHour >> i) & 1);
                        lv_obj_set_style_local_line_opa(_hourLines[i], LV_LINE_PART_MAIN, LV_STATE_DEFAULT, visible ? LV_OPA_COVER : LV_OPA_TRANSP);
                }
        }

        if (minuteChanged)
        {
                // update the visibility of the minute lines
                for (int i = 0; i < 6; i++)
                {
                        bool visible = ((minute() >> i) & 1);
                        lv_obj_set_style_local_line_opa(_minuteLines[i], LV_LINE_PART_MAIN, LV_STATE_DEFAULT, visible ? LV_OPA_COVER : LV_OPA_TRANSP);
                }
        }

        // update the visibility of the second lines
        for (int i = 0; i < 6; i++)
        {
                bool visible = ((second() >> i) & 1);
                lv_obj_set_style_local_line_opa(_secondLines[i], LV_LINE_PART_MAIN, LV_STATE_DEFAULT, visible ? LV_OPA_COVER : LV_OPA_TRANSP);
        }
}


void BinaryWatchFace::lockedStateChanged()
{
        lv_label_set_text_static(_lockSymbol, locked() ? SYMBOL_LOCK : "");
        lv_obj_align(_lockSymbol, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
}


void BinaryWatchFace::updatePowerAndBleSymbols()
{
        lv_label_set_text_static(_powerPresentSymbol, powerPresent() ? SYMBOL_PLUG : "");
        lv_label_set_text_static(_bleSymbol, bleConnected() ? SYMBOL_BLUETOOTH : "");
        lv_obj_align(_powerPresentSymbol, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
        if (powerPresent())
                lv_obj_align(_bleSymbol, _powerPresentSymbol, LV_ALIGN_OUT_LEFT_MID, -4, 0);
        else
                lv_obj_align(_bleSymbol, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
}
