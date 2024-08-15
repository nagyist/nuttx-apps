#
# Copyright (C) 2024 Xiaomi Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#

if(CONFIG_BENCHMARK_TINY_MEMBENCH)

  list(APPEND CFLAGS -Wno-unused-but-set-variable -Wno-unused-variable
       -Wno-strict-prototypes)

  set(SRCS tinymembench/main.c)

  list(
    APPEND
    SRCS
    tinymembench/aarch64-asm.S
    tinymembench/arm-neon.S
    tinymembench/x86-sse2.S
    tinymembench/mips-32.S
    tinymembench/asm-opt.c
    tinymembench/util.c)

  nuttx_add_application(
    NAME
    tinymembench
    PRIORITY
    ${CONFIG_BENCHMARK_TINY_MEMBENCH_PRIORITY}
    STACKSIZE
    ${CONFIG_BENCHMARK_TINY_MEMBENCH_STACKSIZE}
    MODULE
    ${CONFIG_BENCHMARK_TINY_MEMBENCH}
    COMPILE_FLAGS
    ${CFLAGS}
    SRCS
    ${SRCS})
endif()
