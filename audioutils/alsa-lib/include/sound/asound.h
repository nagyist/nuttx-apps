/****************************************************************************
 * apps/audioutils/alsa-lib/include/sound/asound.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ASOUND_ASOUND_H
#define __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ASOUND_ASOUND_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <endian.h>
#include <nuttx/audio/audio.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define SNDRV_PROTOCOL_VERSION(major, minor, subminor)                      \
  (((major) << 16) | ((minor) << 8) | (subminor))

/****************************************************************************
 * Public Types
 ****************************************************************************/

enum sndrv_pcm_format
{
  SNDRV_PCM_FORMAT_MU_LAW = AUDIO_SUBFMT_PCM_MU_LAW,
  SNDRV_PCM_FORMAT_A_LAW  = AUDIO_SUBFMT_PCM_A_LAW,
  SNDRV_PCM_FORMAT_U8     = AUDIO_SUBFMT_PCM_U8,
  SNDRV_PCM_FORMAT_S8     = AUDIO_SUBFMT_PCM_S8,
  SNDRV_PCM_FORMAT_U16_LE = AUDIO_SUBFMT_PCM_U16_LE,
  SNDRV_PCM_FORMAT_S16_BE = AUDIO_SUBFMT_PCM_S16_BE,
  SNDRV_PCM_FORMAT_S16_LE = AUDIO_SUBFMT_PCM_S16_LE,
  SNDRV_PCM_FORMAT_U16_BE = AUDIO_SUBFMT_PCM_U16_BE,
  SNDRV_PCM_FORMAT_U32_LE = AUDIO_SUBFMT_PCM_U32_LE,
  SNDRV_PCM_FORMAT_U32_BE = AUDIO_SUBFMT_PCM_U32_BE,
  SNDRV_PCM_FORMAT_S32_LE = AUDIO_SUBFMT_PCM_S32_LE,
  SNDRV_PCM_FORMAT_S32_BE = AUDIO_SUBFMT_PCM_S32_BE,
  SNDRV_PCM_FORMAT_FIRST  = SNDRV_PCM_FORMAT_MU_LAW,
  SNDRV_PCM_FORMAT_LAST   = SNDRV_PCM_FORMAT_U32_BE,
#if BYTE_ORDER == LITTLE_ENDIAN
  SNDRV_PCM_FORMAT_S16    = SNDRV_PCM_FORMAT_S16_LE,
  SNDRV_PCM_FORMAT_U16    = SNDRV_PCM_FORMAT_U16_LE,
  SNDRV_PCM_FORMAT_S32    = SNDRV_PCM_FORMAT_S32_LE,
  SNDRV_PCM_FORMAT_U32    = SNDRV_PCM_FORMAT_U32_LE,
#else
  SNDRV_PCM_FORMAT_S16    = SNDRV_PCM_FORMAT_S16_BE,
  SNDRV_PCM_FORMAT_U16    = SNDRV_PCM_FORMAT_U16_BE,
  SNDRV_PCM_FORMAT_S32    = SNDRV_PCM_FORMAT_S32_BE,
  SNDRV_PCM_FORMAT_U32    = SNDRV_PCM_FORMAT_U32_BE,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ASOUND_ASOUND_H */