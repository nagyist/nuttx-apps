/**
 * @file vg_lite_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "vg_lite_utils.h"

/*********************
 *      DEFINES
 *********************/

#define ENUM_TO_STRING_DEF(e) \
    case (e):                 \
    return #e

#define VG_ENUM_TO_STRING_CASE_DEF(e) \
    case (VG_LITE_##e):               \
    return #e

#define VLC_OP_ARG_LEN_DEF(OP, LEN) \
    case VLC_OP_##OP:               \
    return (LEN)

#define VLC_GET_OP_CODE(ptr) (*((uint8_t*)ptr))

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

void vg_lite_dump_info(void)
{
    char name[64];
    uint32_t chip_id;
    uint32_t chip_rev;
    uint32_t cid;
    vg_lite_get_product_info(name, &chip_id, &chip_rev);
    vg_lite_get_register(0x30, &cid);
    LV_GPU_LOG_INFO("Product Info: %s"
                    " | Chip ID: 0x%" PRIx32
                    " | Revision: 0x%" PRIx32
                    " | CID: 0x%" PRIx32,
                    name, chip_id, chip_rev, cid);

    vg_lite_info_t info;
    vg_lite_get_info(&info);
    LV_GPU_LOG_INFO("VGLite API version: 0x%" PRIx32, info.api_version);
    LV_GPU_LOG_INFO("VGLite API header version: 0x%" PRIx32, info.header_version);
    LV_GPU_LOG_INFO("VGLite release version: 0x%" PRIx32, info.release_version);

    for(int feature = 0; feature < gcFEATURE_COUNT; feature++) {
        uint32_t ret = vg_lite_query_feature((vg_lite_feature_t)feature);
        LV_UNUSED(ret);
        LV_GPU_LOG_INFO("Feature: %d - %s",
                        feature,
                        ret ? "YES" : "NO");
    }

    uint32_t mem_avail;
    vg_lite_mem_avail(&mem_avail);
    LV_GPU_LOG_INFO("Memory Avaliable: %" PRId32 " Bytes", mem_avail);
}

const char * vg_lite_get_error_type_string(vg_lite_error_t error)
{
    switch(error) {
            VG_ENUM_TO_STRING_CASE_DEF(SUCCESS);
            VG_ENUM_TO_STRING_CASE_DEF(INVALID_ARGUMENT);
            VG_ENUM_TO_STRING_CASE_DEF(OUT_OF_MEMORY);
            VG_ENUM_TO_STRING_CASE_DEF(NO_CONTEXT);
            VG_ENUM_TO_STRING_CASE_DEF(TIMEOUT);
            VG_ENUM_TO_STRING_CASE_DEF(OUT_OF_RESOURCES);
            VG_ENUM_TO_STRING_CASE_DEF(GENERIC_IO);
            VG_ENUM_TO_STRING_CASE_DEF(NOT_SUPPORT);
            VG_ENUM_TO_STRING_CASE_DEF(ALREADY_EXISTS);
            VG_ENUM_TO_STRING_CASE_DEF(NOT_ALIGNED);
            VG_ENUM_TO_STRING_CASE_DEF(FLEXA_TIME_OUT);
            VG_ENUM_TO_STRING_CASE_DEF(FLEXA_HANDSHAKE_FAIL);
        default:
            break;
    }
    return "UNKNOW_ERROR";
}

const char * vg_lite_get_buffer_format_string(vg_lite_buffer_format_t format)
{
    switch(format) {
            VG_ENUM_TO_STRING_CASE_DEF(RGBA8888);
            VG_ENUM_TO_STRING_CASE_DEF(BGRA8888);
            VG_ENUM_TO_STRING_CASE_DEF(RGBX8888);
            VG_ENUM_TO_STRING_CASE_DEF(BGRX8888);
            VG_ENUM_TO_STRING_CASE_DEF(RGB565);
            VG_ENUM_TO_STRING_CASE_DEF(BGR565);
            VG_ENUM_TO_STRING_CASE_DEF(RGBA4444);
            VG_ENUM_TO_STRING_CASE_DEF(BGRA4444);
            VG_ENUM_TO_STRING_CASE_DEF(BGRA5551);
            VG_ENUM_TO_STRING_CASE_DEF(A4);
            VG_ENUM_TO_STRING_CASE_DEF(A8);
            VG_ENUM_TO_STRING_CASE_DEF(L8);
            VG_ENUM_TO_STRING_CASE_DEF(YUYV);
            VG_ENUM_TO_STRING_CASE_DEF(YUY2);
            VG_ENUM_TO_STRING_CASE_DEF(NV12);
            VG_ENUM_TO_STRING_CASE_DEF(ANV12);
            VG_ENUM_TO_STRING_CASE_DEF(AYUY2);
            VG_ENUM_TO_STRING_CASE_DEF(YV12);
            VG_ENUM_TO_STRING_CASE_DEF(YV24);
            VG_ENUM_TO_STRING_CASE_DEF(YV16);
            VG_ENUM_TO_STRING_CASE_DEF(NV16);
            VG_ENUM_TO_STRING_CASE_DEF(YUY2_TILED);
            VG_ENUM_TO_STRING_CASE_DEF(NV12_TILED);
            VG_ENUM_TO_STRING_CASE_DEF(ANV12_TILED);
            VG_ENUM_TO_STRING_CASE_DEF(AYUY2_TILED);
            VG_ENUM_TO_STRING_CASE_DEF(INDEX_1);
            VG_ENUM_TO_STRING_CASE_DEF(INDEX_2);
            VG_ENUM_TO_STRING_CASE_DEF(INDEX_4);
            VG_ENUM_TO_STRING_CASE_DEF(INDEX_8);
            VG_ENUM_TO_STRING_CASE_DEF(RGBA2222);
            VG_ENUM_TO_STRING_CASE_DEF(BGRA2222);
            VG_ENUM_TO_STRING_CASE_DEF(ABGR2222);
            VG_ENUM_TO_STRING_CASE_DEF(ARGB2222);
            VG_ENUM_TO_STRING_CASE_DEF(ABGR4444);
            VG_ENUM_TO_STRING_CASE_DEF(ARGB4444);
            VG_ENUM_TO_STRING_CASE_DEF(ABGR8888);
            VG_ENUM_TO_STRING_CASE_DEF(ARGB8888);
            VG_ENUM_TO_STRING_CASE_DEF(ABGR1555);
            VG_ENUM_TO_STRING_CASE_DEF(RGBA5551);
            VG_ENUM_TO_STRING_CASE_DEF(ARGB1555);
            VG_ENUM_TO_STRING_CASE_DEF(XBGR8888);
            VG_ENUM_TO_STRING_CASE_DEF(XRGB8888);
            VG_ENUM_TO_STRING_CASE_DEF(RGBA8888_ETC2_EAC);
            VG_ENUM_TO_STRING_CASE_DEF(RGB888);
            VG_ENUM_TO_STRING_CASE_DEF(BGR888);
            VG_ENUM_TO_STRING_CASE_DEF(ABGR8565);
            VG_ENUM_TO_STRING_CASE_DEF(BGRA5658);
            VG_ENUM_TO_STRING_CASE_DEF(ARGB8565);
            VG_ENUM_TO_STRING_CASE_DEF(RGBA5658);
        default:
            break;
    }
    return "UNKNOW_BUFFER_FORMAT";
}

const char * vg_lite_get_filter_string(vg_lite_filter_t filter)
{
    switch(filter) {
            VG_ENUM_TO_STRING_CASE_DEF(FILTER_POINT);
            VG_ENUM_TO_STRING_CASE_DEF(FILTER_LINEAR);
            VG_ENUM_TO_STRING_CASE_DEF(FILTER_BI_LINEAR);
        default:
            break;
    }
    return "UNKNOW_FILTER";
}

const char * vg_lite_get_blend_string(vg_lite_blend_t blend)
{
    switch(blend) {
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_NONE); /*! S, i.e. no blending. */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_SRC_OVER); /*! S + (1 - Sa) * D */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_DST_OVER); /*! (1 - Da) * S + D */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_SRC_IN); /*! Da * S */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_DST_IN); /*! Sa * D */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_SCREEN); /*! S + D - S * D */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_MULTIPLY); /*! S * (1 - Da) + D * (1 - Sa) + S * D */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_ADDITIVE); /*! S + D */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_SUBTRACT); /*! D * (1 - S) */
#ifdef CONFIG_LV_GPU_VG_LITE_LVGL_BLEND_SUPPORT
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_SUBTRACT_LVGL); /*! D - S */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_NORMAL_LVGL); /*! S * Sa + (1 - Sa) * D  */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_ADDITIVE_LVGL); /*! (S + D) * Sa + D * (1 - Sa) */
            VG_ENUM_TO_STRING_CASE_DEF(BLEND_MULTIPLY_LVGL); /*! (S * D) * Sa + D * (1 - Sa) */
#endif
        default:
            break;
    }
    return "UNKNOW_BLEND";
}

const char * vg_lite_get_global_alpha_string(vg_lite_global_alpha_t global_alpha)
{
    switch(global_alpha) {
            VG_ENUM_TO_STRING_CASE_DEF(NORMAL);
            VG_ENUM_TO_STRING_CASE_DEF(GLOBAL);
            VG_ENUM_TO_STRING_CASE_DEF(SCALED);
        default:
            break;
    }
    return "UNKNOW_GLOBAL_ALPHA";
}

const char * vg_lite_get_fill_rule_string(vg_lite_fill_t fill_rule)
{
    switch(fill_rule) {
            VG_ENUM_TO_STRING_CASE_DEF(FILL_NON_ZERO);
            VG_ENUM_TO_STRING_CASE_DEF(FILL_EVEN_ODD);
        default:
            break;
    }
    return "UNKNOW_FILL";
}

const char * vg_lite_get_image_mode_string(vg_lite_buffer_image_mode_t image_mode)
{
    switch(image_mode) {
            VG_ENUM_TO_STRING_CASE_DEF(NORMAL_IMAGE_MODE);
            VG_ENUM_TO_STRING_CASE_DEF(NONE_IMAGE_MODE);
            VG_ENUM_TO_STRING_CASE_DEF(MULTIPLY_IMAGE_MODE);
        default:
            break;
    }
    return "UNKNOW_IMAGE_MODE";
}

const char * vg_lite_get_vlc_op_string(uint8_t vlc_op)
{
    switch(vlc_op) {
            ENUM_TO_STRING_DEF(VLC_OP_END);
            ENUM_TO_STRING_DEF(VLC_OP_CLOSE);
            ENUM_TO_STRING_DEF(VLC_OP_MOVE);
            ENUM_TO_STRING_DEF(VLC_OP_MOVE_REL);
            ENUM_TO_STRING_DEF(VLC_OP_LINE);
            ENUM_TO_STRING_DEF(VLC_OP_LINE_REL);
            ENUM_TO_STRING_DEF(VLC_OP_QUAD);
            ENUM_TO_STRING_DEF(VLC_OP_QUAD_REL);
            ENUM_TO_STRING_DEF(VLC_OP_CUBIC);
            ENUM_TO_STRING_DEF(VLC_OP_CUBIC_REL);

            ENUM_TO_STRING_DEF(VLC_OP_SCCWARC);
            ENUM_TO_STRING_DEF(VLC_OP_SCCWARC_REL);
            ENUM_TO_STRING_DEF(VLC_OP_SCWARC);
            ENUM_TO_STRING_DEF(VLC_OP_SCWARC_REL);
            ENUM_TO_STRING_DEF(VLC_OP_LCCWARC);
            ENUM_TO_STRING_DEF(VLC_OP_LCCWARC_REL);
            ENUM_TO_STRING_DEF(VLC_OP_LCWARC);
            ENUM_TO_STRING_DEF(VLC_OP_LCWARC_REL);
        default:
            break;
    }
    return "UNKNOW_VLC_OP";
}

int vg_lite_get_vlc_op_arg_len(uint8_t vlc_op)
{
    switch(vlc_op) {
            VLC_OP_ARG_LEN_DEF(END, 0);
            VLC_OP_ARG_LEN_DEF(CLOSE, 0);
            VLC_OP_ARG_LEN_DEF(MOVE, 2);
            VLC_OP_ARG_LEN_DEF(MOVE_REL, 2);
            VLC_OP_ARG_LEN_DEF(LINE, 2);
            VLC_OP_ARG_LEN_DEF(LINE_REL, 2);
            VLC_OP_ARG_LEN_DEF(QUAD, 4);
            VLC_OP_ARG_LEN_DEF(QUAD_REL, 4);
            VLC_OP_ARG_LEN_DEF(CUBIC, 6);
            VLC_OP_ARG_LEN_DEF(CUBIC_REL, 6);
            VLC_OP_ARG_LEN_DEF(SCCWARC, 5);
            VLC_OP_ARG_LEN_DEF(SCCWARC_REL, 5);
            VLC_OP_ARG_LEN_DEF(SCWARC, 5);
            VLC_OP_ARG_LEN_DEF(SCWARC_REL, 5);
            VLC_OP_ARG_LEN_DEF(LCCWARC, 5);
            VLC_OP_ARG_LEN_DEF(LCCWARC_REL, 5);
            VLC_OP_ARG_LEN_DEF(LCWARC, 5);
            VLC_OP_ARG_LEN_DEF(LCWARC_REL, 5);
        default:
            break;
    }
    LV_GPU_LOG_ERROR("UNKNOW_VLC_OP: 0x%x", vlc_op);
    return 0;
}

int vg_lite_get_path_format_len(vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            return 1;
        case VG_LITE_S16:
            return 2;
        case VG_LITE_S32:
            return 4;
        case VG_LITE_FP32:
            return 4;
        default:
            return 0;
    }
}

static void vg_lite_print_data(const void * data, vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            LV_GPU_LOG_INFO("%d", *((int8_t *)data));
            break;
        case VG_LITE_S16:
            LV_GPU_LOG_INFO("%d", *((int16_t *)data));
            break;
        case VG_LITE_S32:
            LV_GPU_LOG_INFO("%" PRId32, *((int32_t *)data));
            break;
        case VG_LITE_FP32:
            LV_GPU_LOG_INFO("%0.2f", *((float *)data));
            break;
        default:
            LV_GPU_LOG_ERROR("UNKNOW_FORMAT: %d", format);
            break;
    }
}

void vg_lite_dump_path_info(const vg_lite_path_t * path)
{
    LV_ASSERT(path != NULL);
    LV_ASSERT(path->path != NULL);
    int fmt_len = vg_lite_get_path_format_len(path->format);
    LV_ASSERT(fmt_len > 0);

    size_t len = path->path_length / fmt_len;

    LV_ASSERT(len > 0);

    LV_GPU_LOG_INFO("address: %p", path->path);
    LV_GPU_LOG_INFO("length: %d", (int)len);
    LV_GPU_LOG_INFO("bonding box: (%0.2f, %0.2f) - (%0.2f, %0.2f)",
                    path->bounding_box[0], path->bounding_box[1],
                    path->bounding_box[2], path->bounding_box[3]);

    uint8_t * cur = path->path;
    uint8_t * end = cur + path->path_length;

    while(cur < end) {
        /* get op code */
        uint8_t op_code = VLC_GET_OP_CODE(cur);
        LV_GPU_LOG_INFO("%d (%s) ->", op_code, vg_lite_get_vlc_op_string(op_code));

        /* get arguments length */
        int arg_len = vg_lite_get_vlc_op_arg_len(op_code);

        /* skip op code */
        cur += fmt_len;

        /* print arguments */
        while(arg_len--) {
            vg_lite_print_data((uint8_t *)cur, path->format);
            cur += fmt_len;
        }
    }

    uint8_t end_op_code = VLC_GET_OP_CODE(end - fmt_len);
    if(end_op_code != VLC_OP_END) {
        LV_GPU_LOG_ERROR("%d (%s) -> is NOT VLC_OP_END", end_op_code, vg_lite_get_vlc_op_string(end_op_code));
    }
}

void vg_lite_get_buffer_format_bytes(
    vg_lite_buffer_format_t format,
    uint32_t * mul,
    uint32_t * div,
    uint32_t * bytes_align)
{
    /* Get the bpp information of a color format. */
    *mul = *div = 1;
    *bytes_align = 4;
    switch(format) {
        case VG_LITE_L8:
        case VG_LITE_A8:
        case VG_LITE_RGBA8888_ETC2_EAC:
            break;
        case VG_LITE_A4:
            *div = 2;
            break;
        case VG_LITE_ABGR1555:
        case VG_LITE_ARGB1555:
        case VG_LITE_BGRA5551:
        case VG_LITE_RGBA5551:
        case VG_LITE_RGBA4444:
        case VG_LITE_BGRA4444:
        case VG_LITE_ABGR4444:
        case VG_LITE_ARGB4444:
        case VG_LITE_RGB565:
        case VG_LITE_BGR565:
        case VG_LITE_YUYV:
        case VG_LITE_YUY2:
        case VG_LITE_YUY2_TILED:
        /* AYUY2 buffer memory = YUY2 + alpha. */
        case VG_LITE_AYUY2:
        case VG_LITE_AYUY2_TILED:
            *mul = 2;
            break;
        case VG_LITE_RGBA8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_RGBX8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
            *mul = 4;
            break;
        case VG_LITE_NV12:
        case VG_LITE_NV12_TILED:
            *mul = 1;
            break;
        case VG_LITE_ANV12:
        case VG_LITE_ANV12_TILED:
            *mul = 4;
            break;
        case VG_LITE_INDEX_1:
            *div = 8;
            *bytes_align = 8;
            break;
        case VG_LITE_INDEX_2:
            *div = 4;
            *bytes_align = 8;
            break;
        case VG_LITE_INDEX_4:
            *div = 2;
            *bytes_align = 8;
            break;
        case VG_LITE_INDEX_8:
            *bytes_align = 1;
            break;
        case VG_LITE_RGBA2222:
        case VG_LITE_BGRA2222:
        case VG_LITE_ABGR2222:
        case VG_LITE_ARGB2222:
            *mul = 1;
            break;
        case VG_LITE_RGB888:
        case VG_LITE_BGR888:
        case VG_LITE_ABGR8565:
        case VG_LITE_BGRA5658:
        case VG_LITE_ARGB8565:
        case VG_LITE_RGBA5658:
            *mul = 3;
            break;
        default:
            break;
    }
}

void vg_lite_buffer_init(vg_lite_buffer_t * buffer)
{
    memset(buffer, 0, sizeof(vg_lite_buffer_t));
}

vg_lite_buffer_format_t vg_lite_img_cf_to_vg_fmt(lv_img_cf_t cf)
{
    vg_lite_buffer_format_t vg_fmt = VG_LITE_RGBA8888;
    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR:
#if LV_COLOR_DEPTH == 32
            vg_fmt = VG_LITE_BGRX8888;
#elif LV_COLOR_DEPTH == 24
            vg_fmt = VG_LITE_BGR888;
#elif LV_COLOR_DEPTH == 16
            vg_fmt = VG_LITE_BGR565;
#elif LV_COLOR_DEPTH == 8
            /* NOT SUPPORT */
#elif LV_COLOR_DEPTH == 1
            /* NOT SUPPORT */
#endif
            break;
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
#if LV_COLOR_DEPTH == 32
            vg_fmt = VG_LITE_BGRA8888;
#elif LV_COLOR_DEPTH == 24
            vg_fmt = VG_LITE_BGRA8888;
#elif LV_COLOR_DEPTH == 16
            vg_fmt = VG_LITE_BGRA5658;
#elif LV_COLOR_DEPTH == 8
            /* NOT SUPPORT */
#elif LV_COLOR_DEPTH == 1
            /* NOT SUPPORT */
#endif
            break;
        case LV_IMG_CF_INDEXED_1BIT:
            vg_fmt = VG_LITE_INDEX_1;
            break;
        case LV_IMG_CF_INDEXED_2BIT:
            vg_fmt = VG_LITE_INDEX_2;
            break;
        case LV_IMG_CF_INDEXED_4BIT:
            vg_fmt = VG_LITE_INDEX_4;
            break;
        case LV_IMG_CF_INDEXED_8BIT:
            vg_fmt = VG_LITE_INDEX_8;
            break;
        /** The BLEND_SRC_OVER of vg-lite will not premultiply the ALPHA image,
         *  resulting in pure white drawing, so here we use the INDEX8 format instead.
         */
        case LV_IMG_CF_ALPHA_1BIT:
            vg_fmt = VG_LITE_INDEX_1;
            break;
        case LV_IMG_CF_ALPHA_2BIT:
            vg_fmt = VG_LITE_INDEX_2;
            break;
        case LV_IMG_CF_ALPHA_4BIT:
            vg_fmt = VG_LITE_INDEX_4;
            break;
        case LV_IMG_CF_ALPHA_8BIT:
            vg_fmt = VG_LITE_INDEX_8;
            break;
        case LV_IMG_CF_RGB888:
            vg_fmt = VG_LITE_RGB888;
            break;
        case LV_IMG_CF_RGBA8888:
            vg_fmt = VG_LITE_RGBA8888;
            break;
        case LV_IMG_CF_RGBX8888:
            vg_fmt = VG_LITE_RGBX8888;
            break;
        case LV_IMG_CF_RGB565:
            vg_fmt = VG_LITE_RGB565;
            break;
        case LV_IMG_CF_RGBA5658:
            vg_fmt = VG_LITE_RGBA5658;
            break;
        default:
            LV_GPU_LOG_WARN("unsupport color format: %d", cf);
            break;
    }

    return vg_fmt;
}

uint32_t vg_lite_get_palette_size(vg_lite_buffer_format_t format)
{
    uint32_t size = 0;
    switch(format) {
        case VG_LITE_INDEX_1:
            size = 1 << 1;
            break;
        case VG_LITE_INDEX_2:
            size = 1 << 2;
            break;
        case VG_LITE_INDEX_4:
        case VG_LITE_A4:
            size = 1 << 4;
            break;
        case VG_LITE_INDEX_8:
        case VG_LITE_A8:
            size = 1 << 8;
            break;
        default:
            LV_GPU_LOG_WARN("unsupport format: %d(%s)",
                            format,
                            vg_lite_get_buffer_format_string(format));
            break;
    }
    return size;
}

vg_lite_blend_t vg_lite_lv_blend_mode_to_vg_blend_mode(lv_blend_mode_t mode)
{
    vg_lite_blend_t blend;
    switch(mode) {
#ifdef CONFIG_LV_GPU_VG_LITE_LVGL_BLEND_SUPPORT
        case LV_BLEND_MODE_NORMAL: /**< Simply mix according to the opacity value*/
            blend = VG_LITE_BLEND_NORMAL_LVGL;
            break;
        case LV_BLEND_MODE_ADDITIVE: /**< Add the respective color channels*/
            blend = VG_LITE_BLEND_ADDITIVE_LVGL;
            break;
        case LV_BLEND_MODE_SUBTRACTIVE: /**< Subtract the foreground from the background*/
            blend = VG_LITE_BLEND_SUBTRACT_LVGL;
            break;
        case LV_BLEND_MODE_MULTIPLY: /**< Multiply the foreground and background*/
            blend = VG_LITE_BLEND_MULTIPLY_LVGL;
            break;
#else
        case LV_BLEND_MODE_NORMAL: /**< Simply mix according to the opacity value*/
            blend = VG_LITE_BLEND_SRC_OVER;
            break;
        case LV_BLEND_MODE_ADDITIVE: /**< Add the respective color channels*/
            blend = VG_LITE_BLEND_ADDITIVE;
            break;
        case LV_BLEND_MODE_SUBTRACTIVE: /**< Subtract the foreground from the background*/
            blend = VG_LITE_BLEND_SUBTRACT;
            break;
        case LV_BLEND_MODE_MULTIPLY: /**< Multiply the foreground and background*/
            blend = VG_LITE_BLEND_MULTIPLY;
            break;
#endif
        default:
            LV_GPU_LOG_WARN("Unsupport blend mode: %d", mode);
            blend = VG_LITE_BLEND_NONE;
            break;
    }
    return blend;
}

bool vg_lite_check_buffer(const vg_lite_buffer_t * buffer)
{
    uint32_t mul;
    uint32_t div;
    uint32_t align;
    uint32_t stride;

    if(!buffer) {
        LV_LOG_ERROR("buffer is NULL");
        return false;
    }

    if(buffer->width < 1) {
        LV_LOG_ERROR("buffer width(%d) < 1", (int)buffer->width);
        return false;
    }

    if(!VG_LITE_IS_ALIGNED(buffer->width, VG_LITE_IMG_SRC_PX_ALIGN)) {
        LV_LOG_ERROR("buffer width(%d) is not aligned to %d",
                     (int)buffer->width, VG_LITE_IMG_SRC_PX_ALIGN);
        return false;
    }

    if(buffer->height < 1) {
        LV_LOG_ERROR("buffer height(%d) < 1", (int)buffer->height);
        return false;
    }

    if(buffer->stride < 1) {
        LV_LOG_ERROR("buffer stride(%d) < 1", (int)buffer->stride);
        return false;
    }

    if(!(buffer->tiled == VG_LITE_LINEAR || buffer->tiled == VG_LITE_TILED)) {
        LV_LOG_ERROR("buffer tiled(%d) is invalid", (int)buffer->tiled);
        return false;
    }

    if(buffer->memory == NULL) {
        LV_LOG_ERROR("buffer memory is NULL");
        return false;
    }

    if((uintptr_t)buffer->memory != buffer->address) {
        LV_LOG_ERROR("buffer memory(%p) != address(%p)",
                     buffer->memory, (void *)buffer->address);
        return false;
    }

    if(!VG_LITE_IS_ALIGNED(buffer->address, VG_LITE_IMG_SRC_ADDR_ALIGN)) {
        LV_LOG_ERROR("buffer address(%p) is not aligned to %d",
                     (void *)buffer->address, VG_LITE_IMG_SRC_ADDR_ALIGN);
        return false;
    }

    vg_lite_get_buffer_format_bytes(buffer->format, &mul, &div, &align);
    stride = VG_LITE_ALIGN((buffer->width * mul / div), align);

    if(buffer->stride != stride) {
        LV_LOG_ERROR("buffer stride(%d) != %d", (int)buffer->stride, (int)stride);
        return false;
    }

    if(!(buffer->image_mode == VG_LITE_NORMAL_IMAGE_MODE
         || buffer->image_mode == VG_LITE_NONE_IMAGE_MODE
         || buffer->image_mode == VG_LITE_MULTIPLY_IMAGE_MODE)) {
        LV_LOG_ERROR("buffer image_mode(%d) is invalid", (int)buffer->image_mode);
        return false;
    }

    if(!(buffer->transparency_mode == VG_LITE_IMAGE_OPAQUE
         || buffer->transparency_mode == VG_LITE_IMAGE_OPAQUE)) {
        LV_LOG_ERROR("buffer transparency_mode(%d) is invalid",
                     (int)buffer->transparency_mode);
        return false;
    }

    return true;
}

bool vg_lite_check_path(const vg_lite_path_t * path)
{
    if(path == NULL) {
        LV_LOG_ERROR("path is NULL");
        return false;
    }

    if(path->path == NULL) {
        LV_LOG_ERROR("path->path is NULL");
        return false;
    }

    int fmt_len = vg_lite_get_path_format_len(path->format);
    if(fmt_len < 1) {
        LV_LOG_ERROR("path format(%d) is invalid", (int)path->format);
        return false;
    }

    size_t len = path->path_length / fmt_len;

    if(len < 1) {
        LV_LOG_ERROR("path length(%d) error", (int)path->path_length);
        return false;
    }

    uint8_t * cur = path->path;
    uint8_t * end = cur + path->path_length;

    while(cur < end) {
        /* get op code */
        uint8_t op_code = VLC_GET_OP_CODE(cur);

        /* get arguments length */
        int arg_len = vg_lite_get_vlc_op_arg_len(op_code);

        /* skip op code */
        cur += fmt_len;

        /* print arguments */
        while(arg_len--) {
            cur += fmt_len;
        }
    }

    uint8_t end_op_code = VLC_GET_OP_CODE(end - fmt_len);

    if(end_op_code != VLC_OP_END) {
        LV_LOG_ERROR("%d (%s) -> is NOT VLC_OP_END", end_op_code, vg_lite_get_vlc_op_string(end_op_code));
        return false;
    }

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
