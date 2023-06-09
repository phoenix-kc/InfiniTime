#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMAGE_BLACK_BLOCK
#define LV_ATTRIBUTE_IMG_IMAGE_BLACK_BLOCK
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMAGE_BLACK_BLOCK uint8_t image_black_block_map[] = {
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 1*/

  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
};

const lv_img_dsc_t image_black_block = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 9,
  .header.h = 9,
  .data_size = 26,
  .data = image_black_block_map,
};
