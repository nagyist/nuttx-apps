/**
 * @file lv_gpu_buf.h
 *
 */

#ifndef LV_GPU_BUF_H
#define LV_GPU_BUF_H

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

void * gpu_img_alloc(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf, uint32_t * len);

void gpu_img_free(void * img);

lv_img_dsc_t * gpu_img_buf_alloc(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf);

void gpu_img_buf_free(lv_img_dsc_t * dsc);

void gpu_data_update(lv_img_dsc_t * dsc);

uint32_t gpu_img_buf_get_img_size(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf);

void * gpu_data_get_buf(lv_img_dsc_t * dsc);

uint32_t gpu_data_get_buf_size(lv_img_dsc_t * dsc);

void gpu_pre_multiply(lv_color32_t * dst, const lv_color32_t * src, uint32_t count);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_BUF_H*/
