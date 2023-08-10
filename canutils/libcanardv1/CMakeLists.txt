# ##############################################################################
# apps/canutils/libcanardv1/CMakeLists.txt
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

if(CONFIG_CANUTILS_LIBCANARDV1)

  # ############################################################################
  # Config and Fetch libcanardv1 o1heap lib
  # ############################################################################

  set(LIBCANARDV1_DIR ${CMAKE_CURRENT_LIST_DIR}/libcanardv1)
  if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/libcanardv0)
    FetchContent_Declare(
      libcanardv1
      URL ${CONFIG_LIBCANARDV1_URL}/${CONFIG_LIBCANARDV1_VERSION}.zip)
    FetchContent_MakeAvailable(libcanardv1)
    set(LIBCANARDV1_DIR ${libcanardv1_SOURCE_DIR})
  endif()

  set(O1HEAP_DIR ${CMAKE_CURRENT_LIST_DIR}/o1heap)
  if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/o1heap)
    FetchContent_Declare(o1heap
                         URL ${CONFIG_O1HEAP_URL}/${CONFIG_O1HEAP_VERSION}.zip)
    FetchContent_MakeAvailable(o1heap)
    set(O1HEAP_DIR ${o1heap_SOURCE_DIR})
  endif()

  # ############################################################################
  # Library Configuration
  # ############################################################################

  nuttx_add_user_library(libcanardv1)

  target_sources(
    libcanardv1
    PRIVATE ${LIBCANARDV1_DIR}/libcanard/canard.c
            ${LIBCANARDV1_DIR}/libcanard/canard_dsdl.c
            ${O1HEAP_DIR}/o1heap/o1heap.c)
  target_compile_options(
    libcanardv1 PRIVATE -std=c11 -DCANARD_ASSERT=DEBUGASSERT
                        -DCANARD_DSDL_CONFIG_LITTLE_ENDIAN=1)
  target_include_directories(libcanardv1 PRIVATE ${LIBCANARDV1_DIR}/libcanard
                                                 ${O1HEAP_DIR}/o1heap)

endif()
