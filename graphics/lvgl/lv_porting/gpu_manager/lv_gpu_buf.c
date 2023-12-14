/**
 * @file lv_gpu_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gpu_buf.h"

#ifdef CONFIG_LV_GPU_USE_VG_LITE

#include "lv_gpu_conv.h"
#include "vg_lite/vg_lite_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * gpu_img_alloc(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf, uint32_t * len)
{
    return vg_lite_img_alloc(w, h, cf, len);
}

void gpu_img_free(void * img)
{
    vg_lite_img_free(img);
}

lv_img_dsc_t * gpu_img_buf_alloc(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf)
{
    return vg_lite_img_dsc_create(w, h, cf);
}

void gpu_img_buf_free(lv_img_dsc_t * dsc)
{
    vg_lite_img_dsc_del(dsc);
}

void gpu_data_update(lv_img_dsc_t * dsc)
{
    vg_lite_img_dsc_update_header(dsc);
}

uint32_t gpu_img_buf_get_img_size(lv_coord_t w, lv_coord_t h, lv_img_cf_t cf)
{
    return vg_lite_img_buf_get_buf_size(w, h, cf);
}

void * gpu_data_get_buf(lv_img_dsc_t * dsc)
{
    vg_lite_img_header_t * header = (vg_lite_img_header_t *)dsc->data;
    if(header->magic == VG_LITE_IMAGE_MAGIC_NUM) {
        return header->vg_buf.memory;
    }
    LV_GPU_LOG_WARN("NO magic number in dsc: %p", dsc);
    return NULL;
}

uint32_t gpu_data_get_buf_size(lv_img_dsc_t * dsc)
{
    vg_lite_img_header_t * header = (vg_lite_img_header_t *)dsc->data;
    if(header->magic == VG_LITE_IMAGE_MAGIC_NUM) {
        return header->vg_buf.height * header->vg_buf.stride;
    }
    LV_GPU_LOG_WARN("NO magic number in dsc: %p", dsc);
    return 0;
}

void gpu_pre_multiply(lv_color32_t * dst, const lv_color32_t * src, uint32_t count)
{
    lv_gpu_conv_pre_multiply_dsc_t dsc;
    lv_memset_00(&dsc, sizeof(dsc));
    dsc.src = src;
    dsc.dst = dst;
    dsc.count = count;
    lv_gpu_conv_start(LV_GPU_CONV_TYPE_PRE_MULTIPLY, &dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* CONFIG_LV_GPU_USE_VG_LITE */
