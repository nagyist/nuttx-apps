# ##############################################################################
# apps/interpreters/wamr/CMakeLists.txt
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

if(CONFIG_INTERPRETERS_WAMR)

  # ############################################################################
  # Config and Fetch lib
  # ############################################################################

  set(WAMR_DIR ${CMAKE_CURRENT_LIST_DIR}/wamr)

  if(NOT EXISTS ${WAMR_DIR})
    set(WAMR_URL_BASE
        https://github.com/bytecodealliance/wasm-micro-runtime/archive/)

    FetchContent_Declare(
      wamr_fetch
      URL ${WAMR_URL_BASE}/${CONFIG_INTERPRETERS_WAMR_VERSION}.zip SOURCE_DIR
          ${CMAKE_CURRENT_LIST_DIR}/wamr BINARY_DIR
          ${CMAKE_BINARY_DIR}/apps/interpreters/wamr/wamr
      DOWNLOAD_NO_PROGRESS true
      TIMEOUT 30)

    FetchContent_GetProperties(wamr_fetch)

    if(NOT wamr_fetch_POPULATED)
      FetchContent_Populate(wamr_fetch)
    endif()
  endif()

  nuttx_add_library(wamr STATIC)

  set_property(
    TARGET nuttx
    APPEND
    PROPERTY NUTTX_INCLUDE_DIRECTORIES
             ${CMAKE_CURRENT_SOURCE_DIR}/wamr/core/iwasm/include)

  include(${WAMR_DIR}/product-mini/platforms/nuttx/CMakeLists.txt)
  target_sources(wamr PRIVATE ${WAMR_SOURCES})
  target_compile_options(wamr PRIVATE ${WAMR_CFLAGS})
  # the WAMR_INCDIRS and WAMR_DEFINITIONS already exist in the directory domain
  if(CONFIG_INTERPRETERS_IWASM_TASK)
    nuttx_add_application(
      MODULE
      ${CONFIG_INTERPRETERS_WAMR}
      NAME
      iwasm
      STACKSIZE
      ${CONFIG_INTERPRETERS_WAMR_STACKSIZE}
      PRIORITY
      ${CONFIG_INTERPRETERS_WAMR_PRIORITY}
      SRCS
      ${WAMR_DIR}/product-mini/platforms/nuttx/main.c
      COMPILE_FLAGS
      ${WAMR_CFLAGS}
      DEFINITIONS
      ${WAMR_DEFINITIONS}
      INCLUDE_DIRECTORIES
      ${WAMR_INCDIRS}
      DEPENDS
      wamr)
  endif()

  if(CONFIG_INTERPRETERS_WAMR_LIBC_NUTTX OR CONFIG_INTERPRETERS_WAMR_EXTERNAL_MODULE_REGISTRY)
    nuttx_append_source_file_properties(
      ${WAMR_DIR}/product-mini/platforms/nuttx/main.c COMPILE_FLAGS
      -Dwasm_runtime_full_init=wamr_custom_init)
    target_sources(wamr PRIVATE wamr_custom_init.c)
  endif()

  if(CONFIG_INTERPRETERS_WAMR_EXTERNAL_MODULE_REGISTRY)
  set_property(
    TARGET nuttx
    APPEND
    PROPERTY NUTTX_INCLUDE_DIRECTORIES
             ${CMAKE_CURRENT_SOURCE_DIR}/include)
  endif()

  # Add ${CMAKE_BINARY_DIR}/wamrmod to the include directories
  if(CONFIG_INTERPRETERS_WAMR_EXTERNAL_MODULE_REGISTRY)
    target_include_directories(wamr PRIVATE ${CMAKE_BINARY_DIR}/wamrmod)
  endif()

  if(CONFIG_INTERPRETERS_WAMR_LIBC_NUTTX)
    target_sources(wamr PRIVATE libc_nuttx.c)

    set(GLUECSRCS
        ${CMAKE_CURRENT_BINARY_DIR}/syscall_glue.c
        ${CMAKE_CURRENT_BINARY_DIR}/libc_glue.c
        ${CMAKE_CURRENT_BINARY_DIR}/libm_glue.c)

    add_custom_command(
      OUTPUT ${GLUECSRCS}
      COMMAND
        ${NUTTX_DIR}/tools/mkwamrglue.py -i ${NUTTX_DIR}/syscall/syscall.csv -o
        ${CMAKE_CURRENT_BINARY_DIR}/syscall_glue.c
      COMMAND
        ${NUTTX_DIR}/tools/mkwamrglue.py -i ${NUTTX_DIR}/libs/libc/libc.csv -o
        ${CMAKE_CURRENT_BINARY_DIR}/libc_glue.c
      COMMAND
        ${NUTTX_DIR}/tools/mkwamrglue.py -i ${NUTTX_DIR}/libs/libm/libm.csv -o
        ${CMAKE_CURRENT_BINARY_DIR}/libm_glue.c
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      DEPENDS ${NUTTX_DIR}/tools/mkwamrglue.py ${NUTTX_DIR}/syscall/syscall.csv
              ${NUTTX_DIR}/libs/libc/libc.csv ${NUTTX_DIR}/libs/libm/libm.csv
      VERBATIM
      COMMENT "WAMR:Gen and Updating ${GLUECSRCS}")
    target_sources(wamr PRIVATE ${GLUECSRCS})
  endif()

endif()
