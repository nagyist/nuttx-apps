/**
 * @file vg_lite_utils.h
 *
 */

#ifndef VG_LITE_UTILS_H
#define VG_LITE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl/lvgl.h"
#include <vg_lite.h>

/*********************
 *      DEFINES
 *********************/

#define LV_GPU_LOG_TRACE LV_LOG_TRACE
#define LV_GPU_LOG_INFO LV_LOG_INFO
#define LV_GPU_LOG_WARN LV_LOG_WARN
#define LV_GPU_LOG_ERROR LV_LOG_ERROR

#define VG_LITE_IMG_SRC_PX_ALIGN 16
#define VG_LITE_IMG_SRC_ADDR_ALIGN 64

#define VG_LITE_IS_ERROR(err) (err > 0)

#define VG_LITE_CHECK_ERROR(func)                                  \
    do {                                                           \
        error = func;                                              \
        if (VG_LITE_IS_ERROR(error)) {                             \
            LV_GPU_LOG_ERROR("Execute '" #func "' error(%d): %s",  \
                             (int)error, vg_lite_get_error_type_string(error)); \
            goto error_handler;                                    \
        }                                                          \
    } while (0)

#define VG_LITE_ALIGN(number, align_bytes) \
    (((number) + ((align_bytes)-1)) & ~((align_bytes)-1))

#define VG_LITE_IS_ALIGNED(num, align) (((uint32_t)(num) & ((align)-1)) == 0)

#define VG_LITE_DUMP_BUFFER_INFO(buffer)                                                      \
    do {                                                                                      \
        LV_GPU_LOG_TRACE("name: %s", #buffer);                                                \
        LV_GPU_LOG_TRACE("memory: %p", (buffer)->memory);                                     \
        LV_GPU_LOG_TRACE("address: 0x%08x", (int)(buffer)->address);                          \
        LV_GPU_LOG_TRACE("size: W%d x H%d", (int)((buffer)->width), (int)((buffer)->height)); \
        LV_GPU_LOG_TRACE("stride: %d", (int)((buffer)->stride));                              \
        LV_GPU_LOG_TRACE("format: %d (%s)",                                                   \
                         (int)((buffer)->format),                                                          \
                         vg_lite_get_buffer_format_string((buffer)->format));                              \
    } while (0)

#define VG_LITE_DUMP_MATRIX_INFO(matrix)                                  \
    do {                                                                  \
        LV_GPU_LOG_TRACE("name: %s", #matrix);                            \
        for (int i = 0; i < 3; i++) {                                     \
            LV_GPU_LOG_TRACE("| %0.2f, %0.2f, %0.2f |",                   \
                             (matrix)->m[i][0], (matrix)->m[i][1], (matrix)->m[i][2]); \
        }                                                                 \
    } while (0)

#define VG_LITE_IMAGE_MAGIC_NUM 0x7615600D /* VGISGOOD:1981112333 */

#define VG_LITE_IS_INDEX_FMT(fmt) ((fmt) >= VG_LITE_INDEX_1 && (fmt) <= VG_LITE_INDEX_8)

#if LV_COLOR_DEPTH == 32
#define VG_LITE_NATIVE_COLOR_FMT VG_LITE_BGRA8888
#elif LV_COLOR_DEPTH == 24
#define VG_LITE_NATIVE_COLOR_FMT VG_LITE_BGR888
#elif LV_COLOR_DEPTH == 16
#define VG_LITE_NATIVE_COLOR_FMT VG_LITE_BGR565
#else
#error "Unsupport LV_COLOR_DEPTH"
#endif

#ifdef CONFIG_LV_GPU_USE_ASSERT

#define VG_LITE_ASSERT_BUFFER(buffer) LV_ASSERT(vg_lite_check_buffer(buffer))
#define VG_LITE_ASSERT_PATH(path) LV_ASSERT(vg_lite_check_path(path))

#else

#define VG_LITE_ASSERT_BUFFER(buffer)
#define VG_LITE_ASSERT_PATH(path)

#endif /* CONFIG_LV_GPU_USE_ASSERT */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Print info */

void vg_lite_dump_info(void);

const char * vg_lite_get_error_type_string(vg_lite_error_t error);

const char * vg_lite_get_buffer_format_string(vg_lite_buffer_format_t format);

const char * vg_lite_get_filter_string(vg_lite_filter_t filter);

const char * vg_lite_get_blend_string(vg_lite_blend_t blend);

const char * vg_lite_get_global_alpha_string(vg_lite_global_alpha_t global_alpha);

const char * vg_lite_get_fill_rule_string(vg_lite_fill_t fill_rule);

const char * vg_lite_get_image_mode_string(vg_lite_buffer_image_mode_t image_mode);

const char * vg_lite_get_vlc_op_string(uint8_t vlc_op);

int vg_lite_get_vlc_op_arg_len(uint8_t vlc_op);

int vg_lite_get_path_format_len(vg_lite_format_t format);

void vg_lite_dump_path_info(const vg_lite_path_t * path);

/* Converter */

void vg_lite_get_buffer_format_bytes(
    vg_lite_buffer_format_t format,
    uint32_t * mul,
    uint32_t * div,
    uint32_t * bytes_align);

uint32_t vg_lite_get_palette_size(vg_lite_buffer_format_t format);

vg_lite_blend_t vg_lite_lv_blend_mode_to_vg_blend_mode(lv_blend_mode_t mode);

/* Param checker */

bool vg_lite_check_buffer(const vg_lite_buffer_t * buffer);

bool vg_lite_check_path(const vg_lite_path_t * path);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*VG_LITE_UTILS_H*/
