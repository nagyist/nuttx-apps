/**
 * @file vg_lite_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "vg_lite_decoder.h"

#ifdef CONFIG_LV_GPU_USE_VG_LITE

#include <lv_porting/decoder/jpeg_turbo/lv_jpeg_turbo.h>
#include <lv_porting/decoder/lodepng/lv_lodepng.h>
#include "../ext/rle/lvx_rle_decoder.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header);
static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc);
static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc);

static lv_res_t decoder_info_post_processing(
    vg_lite_draw_ctx_t * draw_ctx,
    lv_img_decoder_t * decoder,
    const void * src,
    lv_img_header_t * header);
static lv_res_t decoder_open_post_processing(
    vg_lite_draw_ctx_t * draw_ctx,
    lv_img_decoder_t * decoder,
    lv_img_decoder_dsc_t * dsc);

static lv_img_decoder_t * vg_lite_decoder_create(vg_lite_draw_ctx_t * draw_ctx);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void vg_lite_decoder_init(vg_lite_draw_ctx_t * draw_ctx)
{
    /* create vg_lite decoder */
    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_close_cb(dec, decoder_close);
    dec->user_data = draw_ctx;

    /* create ETC2 decoder */
    if(vg_lite_query_feature(gcFEATURE_BIT_VG_RGBA8_ETC2_EAC)) {
        LV_GPU_LOG_INFO("init decoder: ETC2");
        draw_ctx->etc2_decoder = lv_img_decoder_create();
        vg_lite_decoder_etc2_init(draw_ctx->etc2_decoder);
    }

    /* create EVO decoder */
    LV_GPU_LOG_INFO("init decoder: EVO");
    draw_ctx->evo_decoder = lv_img_decoder_create();
    vg_lite_decoder_evo_init(draw_ctx->evo_decoder);

    /* create RAW data decoder */
    LV_GPU_LOG_INFO("init decoder: RAW");
    draw_ctx->raw_decoder = lv_img_decoder_create();
    vg_lite_decoder_raw_init(draw_ctx->raw_decoder);

    /* vg_lite decoder list */
    _lv_ll_init(&draw_ctx->decoder_ll, sizeof(lv_img_decoder_t));

    (void)vg_lite_decoder_create;

#ifdef CONFIG_LV_USE_DECODER_JPEG_TURBO
    lv_jpeg_turbo_custom_init(vg_lite_decoder_create(draw_ctx));
#endif

#ifdef CONFIG_LV_USE_DECODER_LODEPNG
    lv_lodepng_custom_init(vg_lite_decoder_create(draw_ctx));
#endif

#ifdef CONFIG_LVX_USE_RLE
    lvx_rle_decoder_custom_init(vg_lite_decoder_create(draw_ctx));
#endif
}

void vg_lite_decoder_uninit(vg_lite_draw_ctx_t * draw_ctx)
{
    LV_GPU_LOG_TRACE("clear image cache");
    lv_img_cache_invalidate_src(NULL);

    LV_GPU_LOG_TRACE("delete decoders");
    if(draw_ctx->etc2_decoder) {
        lv_img_decoder_delete(draw_ctx->etc2_decoder);
    }
    lv_img_decoder_delete(draw_ctx->evo_decoder);
    lv_img_decoder_delete(draw_ctx->raw_decoder);

    LV_GPU_LOG_TRACE("clear decoder list");
    lv_img_decoder_t * dec;
    _LV_LL_READ(&draw_ctx->decoder_ll, dec) {
        lv_img_decoder_delete(dec);
    }
    _lv_ll_clear(&draw_ctx->decoder_ll);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_img_decoder_t * vg_lite_decoder_create(vg_lite_draw_ctx_t * draw_ctx)
{
    lv_img_decoder_t * decoder;
    decoder = _lv_ll_ins_head(&draw_ctx->decoder_ll);
    LV_ASSERT_MALLOC(decoder);
    lv_memset_00(decoder, sizeof(lv_img_decoder_t));
    return decoder;
}

static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header)
{
    vg_lite_draw_ctx_t * draw_ctx = decoder->user_data;

    lv_img_decoder_t * dec;
    lv_res_t res = LV_RES_INV;
    _LV_LL_READ(&draw_ctx->decoder_ll, dec) {
        LV_ASSERT_NULL(dec->info_cb);
        res = dec->info_cb(dec, src, header);
        if(res == LV_RES_OK) {
            res = decoder_info_post_processing(draw_ctx, dec, src, header);
            break;
        }
    }
    return res;
}

static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    vg_lite_draw_ctx_t * draw_ctx = decoder->user_data;
    lv_img_decoder_t * dec;
    lv_res_t res = LV_RES_INV;
    _LV_LL_READ(&draw_ctx->decoder_ll, dec) {
        LV_ASSERT_NULL(dec->open_cb);
        res = dec->open_cb(dec, dsc);
        if(res == LV_RES_OK) {
            res = decoder_open_post_processing(draw_ctx, dec, dsc);
            break;
        }
    }
    return res;
}

static lv_res_t decoder_info_post_processing(vg_lite_draw_ctx_t * draw_ctx, lv_img_decoder_t * decoder,
                                             const void * src, lv_img_header_t * header)
{
    return LV_RES_OK;
}

static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    vg_lite_draw_ctx_t * draw_ctx = decoder->user_data;
    LV_ASSERT_NULL(draw_ctx->raw_decoder->close_cb);
    draw_ctx->raw_decoder->close_cb(draw_ctx->raw_decoder, dsc);
}

static lv_res_t decoder_open_post_processing(vg_lite_draw_ctx_t * draw_ctx, lv_img_decoder_t * decoder,
                                             lv_img_decoder_dsc_t * dsc)
{
    /* create a temporary raw image */
    lv_img_dsc_t img_dsc;
    lv_memset_00(&img_dsc, sizeof(img_dsc));
    img_dsc.header = dsc->header;
    img_dsc.data = dsc->img_data;

    lv_img_decoder_dsc_t new_dsc;
    lv_memcpy(&new_dsc, dsc, sizeof(new_dsc));
    new_dsc.decoder = draw_ctx->raw_decoder;
    new_dsc.img_data = NULL;
    new_dsc.src = &img_dsc;
    new_dsc.src_type = LV_IMG_SRC_VARIABLE;

    /* decode raw image */
    LV_ASSERT_NULL(draw_ctx->raw_decoder->open_cb);
    lv_res_t res = draw_ctx->raw_decoder->open_cb(draw_ctx->raw_decoder, &new_dsc);
    LV_ASSERT(res == LV_RES_OK);

    /* Copy source info */
    new_dsc.src_type = dsc->src_type;
    new_dsc.src = dsc->src;

    /* FIXME: prevent dsc->src be free'd */
    dsc->src_type = LV_IMG_SRC_VARIABLE;
    dsc->decoder = decoder;
    lv_img_decoder_close(dsc);

    /* change to raw_decoder image data */
    lv_memcpy(dsc, &new_dsc, sizeof(new_dsc));

    return LV_RES_OK;
}

#endif /* CONFIG_LV_GPU_USE_VG_LITE */
