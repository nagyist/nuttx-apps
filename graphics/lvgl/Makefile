############################################################################
# apps/graphics/lvgl/Makefile
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

# LVGL graphic library

LVGL_DIR = .
LVGL_DIR_NAME = lvgl

SUFFIX := lvgl

# Relax format check for LVGL to avoid errors on prinf() use

-include ./lvgl/lvgl.mk

-include ./ext/Makefile

ifneq ($(CONFIG_LV_USE_LIBPNG),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/libpng
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/libpng/libpng
endif

ifneq ($(CONFIG_LV_USE_LIBWEBP),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/libwebp/libwebp/src/
endif

ifneq ($(CONFIG_LV_USE_LIBJPEG_TURBO),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/libjpeg-turbo/libjpeg-turbo
endif

ifneq ($(CONFIG_LV_USE_FFMPEG),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/ffmpeg/ffmpeg
endif

ifneq ($(CONFIG_LV_USE_RLOTTIE),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/rlottie/rlottie/inc
endif

ifneq ($(CONFIG_LVX_USE_QRSCAN),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/quirc/quirc/lib
endif

ifneq ($(CONFIG_LV_ASSERT_HANDLER_INCLUDE), "")
CFLAGS += "-DLV_ASSERT_HANDLER=ASSERT(0);"
endif

ifeq ($(and $(CONFIG_LV_USE_PERF_MONITOR),$(CONFIG_SCHED_CPULOAD)),y)
CFLAGS += "-DLV_SYSMON_GET_IDLE=lv_nuttx_get_idle"
endif

ifeq ($(and $(CONFIG_SCHED_INSTRUMENTATION),$(CONFIG_LV_USE_PROFILER)),y)
CFLAGS += "-DLV_PROFILER_BEGIN=sched_note_beginex(NOTE_TAG_GRAPHICS, __func__)"
CFLAGS += "-DLV_PROFILER_END=sched_note_endex(NOTE_TAG_GRAPHICS, __func__)"

CFLAGS += "-DLV_PROFILER_BEGIN_TAG(str)=sched_note_beginex(NOTE_TAG_GRAPHICS, str)"
CFLAGS += "-DLV_PROFILER_END_TAG(str)=sched_note_endex(NOTE_TAG_GRAPHICS, str)"
endif

ifneq ($(CONFIG_LV_GPU_G2D_INCLUDE_PATH),)
CFLAGS += "-I$(addprefix $(strip $(INCDIR_PREFIX))$(APPDIR)/../,$(CONFIG_LV_GPU_G2D_INCLUDE_PATH))"
endif

ifeq ($(CONFIG_LV_USE_DRAW_VG_LITE),y)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/../$(CONFIG_LV_DRAW_VG_LITE_INCLUDE)

# Disable optimization for vg_lite_path.c to avoid a compiler bug.
# See: https://bugs.launchpad.net/gcc-arm-embedded/+bug/1982289
$(CURDIR)/lvgl/src/draw/vg_lite/lv_vg_lite_path.c_CFLAGS += -O0
endif

ifneq ($(CONFIG_LV_OPTLEVEL), "")
# Since multiple options need to be supported, subst needs to be used here to remove
# the redundant double quotes, otherwise it will cause command parsing errors.
CFLAGS   += $(subst ",, $(CONFIG_LV_OPTLEVEL))
CXXFLAGS += $(subst ",, $(CONFIG_LV_OPTLEVEL))
endif

include $(APPDIR)/Application.mk
