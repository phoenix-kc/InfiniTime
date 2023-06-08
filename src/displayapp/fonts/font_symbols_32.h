#ifndef FONT_SYMBOLS_32_H
#define FONT_SYMBOLS_32_H


#include <lvgl/lvgl.h>


LV_FONT_DECLARE(font_symbols_32);


// symbol cheatsheet:         https://fontawesome.com/v5/cheatsheet/free/solid
// symbol UTF-8 conversion:   https://mothereff.in/utf-8

#define SYMBOL_SUN                "\xEF\x86\x85"   // 0xf185
#define SYMBOL_SNOWFLAKE          "\xEF\x8B\x9C"   // 0xf2dc
#define SYMBOL_CLOUD              "\xEF\x83\x82"   // 0xf0c2
#define SYMBOL_CLOUD_RAIN         "\xEF\x9C\xBD"   // 0xf73d
#define SYMBOL_CLOUD_HEAVY_RAIN   "\xEF\x9D\x80"   // 0xf740
#define SYMBOL_COULD_MOON         "\xEF\x9B\x83"   // 0xf6c3
#define SYMBOL_CLOUD_MOON_RAIN    "\xEF\x9C\xBC"   // 0xf73c
#define SYMBOL_CLOUD_SUN          "\xEF\x9B\x84"   // 0xf6c4
#define SYMBOL_CLOUD_SUN_RAIN     "\xEF\x9D\x83"   // 0xf743
#define SYMBOL_WALKING            "\xEF\x95\x94"   // 0xf554


// contained range:
// 0xf0c2, 0xf185, 0xf2dc, 0xf554, 0xf6c3, 0xf6c4, 0xf73c, 0xf73d, 0xf740, 0xf743

#endif // FONT_SYMBOLS_32_H
