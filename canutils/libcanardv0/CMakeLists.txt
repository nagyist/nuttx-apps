# ##############################################################################
# apps/canutils/libcanardv0/CMakeLists.txt
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for
# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

if(CONFIG_CANUTILS_LIBCANARDV0)

  # ############################################################################
  # Config and Fetch libcanardv0 lib
  # ############################################################################

  set(LIBCANARDV0_DIR ${CMAKE_CURRENT_LIST_DIR}/libcanardv0)
  if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/libcanardv0)
    FetchContent_Declare(
      libcanardv0
      URL ${CONFIG_LIBCANARDV0_URL}/${CONFIG_LIBCANARDV0_VERSION}.zip)
    FetchContent_MakeAvailable(libcanardv0)
    set(LIBCANARDV0_DIR ${libcanardv0_SOURCE_DIR})
  endif()

  # ############################################################################
  # Library Configuration
  # ############################################################################

  nuttx_add_user_library(libcanardv0)

  target_sources(
    libcanardv0 PRIVATE ${LIBCANARDV0_DIR}/canard.c
                        ${LIBCANARDV0_DIR}/drivers/nuttx/canard_nuttx.c)
  target_compile_options(libcanardv0 PRIVATE -std=c99
                                             -DCANARD_ASSERT=DEBUGASSERT)
  target_include_directories(
    libcanardv0 PRIVATE ${LIBCANARDV0_DIR} ${LIBCANARDV0_DIR}/drivers/nuttx)

endif()
