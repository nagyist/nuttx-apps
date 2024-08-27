#
# Copyright (C) 2024 Xiaomi Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

include $(APPDIR)/Make.defs

PROGNAME    = tinymembench
PRIORITY    = $(CONFIG_BENCHMARK_TINY_MEMBENCH_PRIORITY)
STACKSIZE   = $(CONFIG_BENCHMARK_TINY_MEMBENCH_STACKSIZE)
MODULE      = $(CONFIG_BENCHMARK_TINY_MEMBENCH)

ASRCS      += tinymembench/aarch64-asm.S
ASRCS      += tinymembench/arm-neon.S
ASRCS      += tinymembench/x86-sse2.S
ASRCS      += tinymembench/mips-32.S

CSRCS      += tinymembench/asm-opt.c
CSRCS      += tinymembench/util.c

MAINSRC     = tinymembench/main.c

CFLAGS     += -Wno-unused-but-set-variable -Wno-unused-variable
CFLAGS     += -Wno-strict-prototypes

include $(APPDIR)/Application.mk
