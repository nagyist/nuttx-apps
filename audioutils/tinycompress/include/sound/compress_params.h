/****************************************************************************
 * apps/audioutils/tinycompress/include/sound/compress_params.h
 *
 * This header was automatically generated from a Linux kernel header
 * of the same name, to make information necessary for userspace to
 * call into the kernel available to libc.  It contains only constants,
 * structures, and macros generated from the original header, and thus,
 * contains no copyrightable information.
 *
 * To edit the content of this header, modify the corresponding
 * source file (e.g. under external/kernel-headers/original/) then
 * run bionic/libc/kernel/tools/update_all.py
 *
 * Any manual change here will be lost the next time this script will
 * be run. You've been warned!
 *
 ****************************************************************************/

#ifndef __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_PARAMS_H
#define __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_PARAMS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define MAX_NUM_CODECS 64
#define MAX_NUM_CODEC_DESCRIPTORS 32
#define MAX_NUM_BITRATES 32
#define MAX_NUM_SAMPLE_RATES 32

#define SND_AUDIOCODEC_PCM ((uint32_t)0x00000001)
#define SND_AUDIOCODEC_MP3 ((uint32_t)0x00000002)
#define SND_AUDIOCODEC_AMR ((uint32_t)0x00000003)
#define SND_AUDIOCODEC_AMRWB ((uint32_t)0x00000004)
#define SND_AUDIOCODEC_AMRWBPLUS ((uint32_t)0x00000005)
#define SND_AUDIOCODEC_AAC ((uint32_t)0x00000006)
#define SND_AUDIOCODEC_WMA ((uint32_t)0x00000007)
#define SND_AUDIOCODEC_REAL ((uint32_t)0x00000008)
#define SND_AUDIOCODEC_VORBIS ((uint32_t)0x00000009)
#define SND_AUDIOCODEC_FLAC ((uint32_t)0x0000000A)
#define SND_AUDIOCODEC_IEC61937 ((uint32_t)0x0000000B)
#define SND_AUDIOCODEC_G723_1 ((uint32_t)0x0000000C)
#define SND_AUDIOCODEC_G729 ((uint32_t)0x0000000D)
#define SND_AUDIOCODEC_BESPOKE ((uint32_t)0x0000000E)
#define SND_AUDIOCODEC_ALAC ((uint32_t)0x0000000F)
#define SND_AUDIOCODEC_APE ((uint32_t)0x00000010)
#define SND_AUDIOCODEC_OPUS ((uint32_t)0x00000011)

#define SND_AUDIOCODEC_MAX SND_AUDIOCODEC_OPUS
#define SND_AUDIOPROFILE_PCM ((uint32_t)0x00000001)

#define SND_AUDIOCHANMODE_MP3_MONO ((uint32_t)0x00000001)
#define SND_AUDIOCHANMODE_MP3_STEREO ((uint32_t)0x00000002)
#define SND_AUDIOCHANMODE_MP3_JOINTSTEREO ((uint32_t)0x00000004)
#define SND_AUDIOCHANMODE_MP3_DUAL ((uint32_t)0x00000008)

#define SND_AUDIOPROFILE_AMR ((uint32_t)0x00000001)

#define SND_AUDIOMODE_AMR_DTX_OFF ((uint32_t)0x00000001)
#define SND_AUDIOMODE_AMR_VAD1 ((uint32_t)0x00000002)
#define SND_AUDIOMODE_AMR_VAD2 ((uint32_t)0x00000004)

#define SND_AUDIOSTREAMFORMAT_UNDEFINED ((uint32_t)0x00000000)
#define SND_AUDIOSTREAMFORMAT_CONFORMANCE ((uint32_t)0x00000001)
#define SND_AUDIOSTREAMFORMAT_IF1 ((uint32_t)0x00000002)
#define SND_AUDIOSTREAMFORMAT_IF2 ((uint32_t)0x00000004)
#define SND_AUDIOSTREAMFORMAT_FSF ((uint32_t)0x00000008)
#define SND_AUDIOSTREAMFORMAT_RTPPAYLOAD ((uint32_t)0x00000010)
#define SND_AUDIOSTREAMFORMAT_ITU ((uint32_t)0x00000020)

#define SND_AUDIOPROFILE_AMRWB ((uint32_t)0x00000001)

#define SND_AUDIOMODE_AMRWB_DTX_OFF ((uint32_t)0x00000001)
#define SND_AUDIOMODE_AMRWB_VAD1 ((uint32_t)0x00000002)
#define SND_AUDIOMODE_AMRWB_VAD2 ((uint32_t)0x00000004)

#define SND_AUDIOPROFILE_AMRWBPLUS ((uint32_t)0x00000001)

#define SND_AUDIOPROFILE_AAC ((uint32_t)0x00000001)

#define SND_AUDIOMODE_AAC_MAIN ((uint32_t)0x00000001)
#define SND_AUDIOMODE_AAC_LC ((uint32_t)0x00000002)
#define SND_AUDIOMODE_AAC_SSR ((uint32_t)0x00000004)
#define SND_AUDIOMODE_AAC_LTP ((uint32_t)0x00000008)
#define SND_AUDIOMODE_AAC_HE ((uint32_t)0x00000010)
#define SND_AUDIOMODE_AAC_SCALABLE ((uint32_t)0x00000020)
#define SND_AUDIOMODE_AAC_ERLC ((uint32_t)0x00000040)
#define SND_AUDIOMODE_AAC_LD ((uint32_t)0x00000080)
#define SND_AUDIOMODE_AAC_HE_PS ((uint32_t)0x00000100)
#define SND_AUDIOMODE_AAC_HE_MPS ((uint32_t)0x00000200)

#define SND_AUDIOSTREAMFORMAT_MP2ADTS ((uint32_t)0x00000001)
#define SND_AUDIOSTREAMFORMAT_MP4ADTS ((uint32_t)0x00000002)
#define SND_AUDIOSTREAMFORMAT_MP4LOAS ((uint32_t)0x00000004)
#define SND_AUDIOSTREAMFORMAT_MP4LATM ((uint32_t)0x00000008)
#define SND_AUDIOSTREAMFORMAT_ADIF ((uint32_t)0x00000010)
#define SND_AUDIOSTREAMFORMAT_MP4FF ((uint32_t)0x00000020)
#define SND_AUDIOSTREAMFORMAT_RAW ((uint32_t)0x00000040)

#define SND_AUDIOPROFILE_WMA7 ((uint32_t)0x00000001)
#define SND_AUDIOPROFILE_WMA8 ((uint32_t)0x00000002)
#define SND_AUDIOPROFILE_WMA9 ((uint32_t)0x00000004)
#define SND_AUDIOPROFILE_WMA10 ((uint32_t)0x00000008)
#define SND_AUDIOPROFILE_WMA9_PRO ((uint32_t)0x00000010)
#define SND_AUDIOPROFILE_WMA9_LOSSLESS ((uint32_t)0x00000020)
#define SND_AUDIOPROFILE_WMA10_LOSSLESS ((uint32_t)0x00000040)

#define SND_AUDIOMODE_WMA_LEVEL1 ((uint32_t)0x00000001)
#define SND_AUDIOMODE_WMA_LEVEL2 ((uint32_t)0x00000002)
#define SND_AUDIOMODE_WMA_LEVEL3 ((uint32_t)0x00000004)
#define SND_AUDIOMODE_WMA_LEVEL4 ((uint32_t)0x00000008)
#define SND_AUDIOMODE_WMAPRO_LEVELM0 ((uint32_t)0x00000010)
#define SND_AUDIOMODE_WMAPRO_LEVELM1 ((uint32_t)0x00000020)
#define SND_AUDIOMODE_WMAPRO_LEVELM2 ((uint32_t)0x00000040)
#define SND_AUDIOMODE_WMAPRO_LEVELM3 ((uint32_t)0x00000080)

#define SND_AUDIOSTREAMFORMAT_WMA_ASF ((uint32_t)0x00000001)

#define SND_AUDIOSTREAMFORMAT_WMA_NOASF_HDR ((uint32_t)0x00000002)

#define SND_AUDIOPROFILE_REALAUDIO ((uint32_t)0x00000001)

#define SND_AUDIOMODE_REALAUDIO_G2 ((uint32_t)0x00000001)
#define SND_AUDIOMODE_REALAUDIO_8 ((uint32_t)0x00000002)
#define SND_AUDIOMODE_REALAUDIO_10 ((uint32_t)0x00000004)
#define SND_AUDIOMODE_REALAUDIO_SURROUND ((uint32_t)0x00000008)

#define SND_AUDIOPROFILE_VORBIS ((uint32_t)0x00000001)

#define SND_AUDIOMODE_VORBIS ((uint32_t)0x00000001)

#define SND_AUDIOPROFILE_FLAC ((uint32_t)0x00000001)

#define SND_AUDIOMODE_FLAC_LEVEL0 ((uint32_t)0x00000001)
#define SND_AUDIOMODE_FLAC_LEVEL1 ((uint32_t)0x00000002)
#define SND_AUDIOMODE_FLAC_LEVEL2 ((uint32_t)0x00000004)
#define SND_AUDIOMODE_FLAC_LEVEL3 ((uint32_t)0x00000008)
#define SND_AUDIOMODE_FLAC_LEVEL4 ((uint32_t)0x00000010)
#define SND_AUDIOMODE_FLAC_LEVEL5 ((uint32_t)0x00000020)
#define SND_AUDIOMODE_FLAC_LEVEL6 ((uint32_t)0x00000040)
#define SND_AUDIOMODE_FLAC_LEVEL7 ((uint32_t)0x00000080)
#define SND_AUDIOMODE_FLAC_LEVEL8 ((uint32_t)0x00000100)

#define SND_AUDIOSTREAMFORMAT_FLAC ((uint32_t)0x00000001)
#define SND_AUDIOSTREAMFORMAT_FLAC_OGG ((uint32_t)0x00000002)

#define SND_AUDIOPROFILE_IEC61937 ((uint32_t)0x00000001)

#define SND_AUDIOPROFILE_IEC61937_SPDIF ((uint32_t)0x00000002)

#define SND_AUDIOMODE_IEC_REF_STREAM_HEADER ((uint32_t)0x00000000)
#define SND_AUDIOMODE_IEC_LPCM ((uint32_t)0x00000001)
#define SND_AUDIOMODE_IEC_AC3 ((uint32_t)0x00000002)
#define SND_AUDIOMODE_IEC_MPEG1 ((uint32_t)0x00000004)
#define SND_AUDIOMODE_IEC_MP3 ((uint32_t)0x00000008)
#define SND_AUDIOMODE_IEC_MPEG2 ((uint32_t)0x00000010)
#define SND_AUDIOMODE_IEC_AACLC ((uint32_t)0x00000020)
#define SND_AUDIOMODE_IEC_DTS ((uint32_t)0x00000040)
#define SND_AUDIOMODE_IEC_ATRAC ((uint32_t)0x00000080)
#define SND_AUDIOMODE_IEC_SACD ((uint32_t)0x00000100)
#define SND_AUDIOMODE_IEC_EAC3 ((uint32_t)0x00000200)
#define SND_AUDIOMODE_IEC_DTS_HD ((uint32_t)0x00000400)
#define SND_AUDIOMODE_IEC_MLP ((uint32_t)0x00000800)
#define SND_AUDIOMODE_IEC_DST ((uint32_t)0x00001000)
#define SND_AUDIOMODE_IEC_WMAPRO ((uint32_t)0x00002000)
#define SND_AUDIOMODE_IEC_REF_CXT ((uint32_t)0x00004000)
#define SND_AUDIOMODE_IEC_HE_AAC ((uint32_t)0x00008000)
#define SND_AUDIOMODE_IEC_HE_AAC2 ((uint32_t)0x00010000)
#define SND_AUDIOMODE_IEC_MPEG_SURROUND ((uint32_t)0x00020000)

#define SND_AUDIOPROFILE_G723_1 ((uint32_t)0x00000001)

#define SND_AUDIOMODE_G723_1_ANNEX_A ((uint32_t)0x00000001)
#define SND_AUDIOMODE_G723_1_ANNEX_B ((uint32_t)0x00000002)
#define SND_AUDIOMODE_G723_1_ANNEX_C ((uint32_t)0x00000004)

#define SND_AUDIOPROFILE_G729 ((uint32_t)0x00000001)

#define SND_AUDIOMODE_G729_ANNEX_A ((uint32_t)0x00000001)
#define SND_AUDIOMODE_G729_ANNEX_B ((uint32_t)0x00000002)

#define SND_RATECONTROLMODE_CONSTANTBITRATE ((uint32_t)0x00000001)
#define SND_RATECONTROLMODE_VARIABLEBITRATE ((uint32_t)0x00000002)

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct snd_enc_wma
{
  uint32_t super_block_align;
};

struct snd_enc_vorbis
{
  int32_t quality;
  uint32_t managed;
  uint32_t max_bit_rate;
  uint32_t min_bit_rate;
  uint32_t downmix;
};

struct snd_enc_real
{
  uint32_t quant_bits;
  uint32_t start_region;
  uint32_t num_regions;
};

struct snd_enc_flac
{
  uint32_t num;
  uint32_t gain;
};

struct snd_enc_generic
{
  uint32_t bw;
  int32_t reserved[15];
};

struct snd_dec_flac
{
  uint16_t sample_size;
  uint16_t min_blk_size;
  uint16_t max_blk_size;
  uint16_t min_frame_size;
  uint16_t max_frame_size;
  uint16_t reserved;
};

struct snd_dec_wma
{
  uint32_t encoder_option;
  uint32_t adv_encoder_option;
  uint32_t adv_encoder_option2;
  uint32_t reserved;
};

struct snd_dec_alac
{
  uint32_t frame_length;
  uint8_t compatible_version;
  uint8_t pb;
  uint8_t mb;
  uint8_t kb;
  uint32_t max_run;
  uint32_t max_frame_bytes;
};

struct snd_dec_ape
{
  uint16_t compatible_version;
  uint16_t compression_level;
  uint32_t format_flags;
  uint32_t blocks_per_frame;
  uint32_t final_frame_blocks;
  uint32_t total_frames;
  uint32_t seek_table_present;
};

union snd_codec_options
{
  struct snd_enc_wma wma;
  struct snd_enc_vorbis vorbis;
  struct snd_enc_real real;
  struct snd_enc_flac flac;
  struct snd_enc_generic generic;
  struct snd_dec_flac flac_d;
  struct snd_dec_wma wma_d;
  struct snd_dec_alac alac_d;
  struct snd_dec_ape ape_d;
};

struct snd_codec_desc
{
  uint32_t max_ch;
  uint32_t sample_rates[MAX_NUM_SAMPLE_RATES];
  uint32_t num_sample_rates;
  uint32_t bit_rate[MAX_NUM_BITRATES];
  uint32_t num_bitrates;
  uint32_t rate_control;
  uint32_t profiles;
  uint32_t modes;
  uint32_t formats;
  uint32_t min_buffer;
  uint32_t reserved[15];
};

struct snd_codec
{
  uint32_t id;
  uint32_t ch_in;
  uint32_t ch_out;
  uint32_t sample_rate;
  uint32_t bit_rate;
  uint32_t rate_control;
  uint32_t profile;
  uint32_t level;
  uint32_t ch_mode;
  uint32_t format;
  uint32_t align;
  union snd_codec_options options;
  uint32_t reserved[3];
};

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_PARAMS_H */
