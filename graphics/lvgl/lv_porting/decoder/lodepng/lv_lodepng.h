/**
 * @file lv_lodepng.h
 *
 */

#ifndef LV_LODEPNG_H
#define LV_LODEPNG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <lvgl/lvgl.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register the PNG decoder functions in LVGL
 */
void lv_lodepng_custom_init(lv_img_decoder_t * dec);

static inline void lv_lodepng_init(void)
{
    lv_lodepng_custom_init(lv_img_decoder_create());
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LODEPNG_H*/
