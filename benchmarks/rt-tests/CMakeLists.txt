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

if(CONFIG_BENCHMARK_RTTESTS)

  list(
    APPEND
    SRCS
    rt-tests/src/cyclictest/cyclictest.c
    rt-tests/src/lib/rt-error.c
    rt-tests/src/lib/histogram.c
    rt-tests/src/lib/rt-utils.c
    rt-tests/src/lib/rt-numa.c)

  list(
    APPEND
    CFLAGS
    -Irt-tests/src/include
    -I.
    -DVERSION=0.1
    -DSYS_gettid=0
    -Dgettid=rttest_gettid
    -D'syscall
    (x)
    =
    ((pid_t) (x))
    '
    -w
    -DRTTESTS_PRIORITY=${CONFIG_BENCHMARK_RTTESTS_PRIORITY}+1)

  nuttx_add_application(
    NAME
    cyclictest
    PRIORITY
    ${CONFIG_BENCHMARK_RTTESTS_PRIORITY}
    STACKSIZE
    ${CONFIG_BENCHMARK_RTTESTS_STACKSIZE}
    MODULE
    ${CONFIG_BENCHMARK_RTTESTS}
    COMPILE_FLAGS
    ${CFLAGS}
    SRCS
    ${SRCS})
endif()
