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

############################################################################
# Targets
############################################################################


include $(APPDIR)/Make.defs

PROGNAME  = cyclictest
PRIORITY  = $(CONFIG_BENCHMARK_RTTESTS_PRIORITY)
STACKSIZE = $(CONFIG_BENCHMARK_RTTESTS_STACKSIZE)
MODULE    = $(CONFIG_BENCHMARK_RTTESTS)

MAINSRC   = rt-tests/src/cyclictest/cyclictest.c

CFLAGS   += -Irt-tests/src/include
CFLAGS   += -Wno-maybe-uninitialized -Wno-unused-variable -Wno-unused-function
CFLAGS   += -Wno-unused-but-set-variable -Wno-shadow -Wno-stringop-truncation
CFLAGS   += -Wno-strict-prototypes

include $(APPDIR)/Application.mk
