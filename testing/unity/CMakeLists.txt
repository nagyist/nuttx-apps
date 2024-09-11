# ##############################################################################
# apps/testing/unity/CMakeLists.txt
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

if(CONFIG_TESTING_UNITY)

  set(UNITY_DIR ${CMAKE_CURRENT_LIST_DIR}/unity)

  if(NOT EXISTS ${UNITY_DIR})

    FetchContent_Declare(
      unity_fetch
      URL ${CONFIG_TESTING_UNITY_URL}/${CONFIG_TESTING_UNITY_VERSION} SOURCE_DIR
          ${CMAKE_CURRENT_LIST_DIR}/unity BINARY_DIR
          ${CMAKE_BINARY_DIR}/apps/testing/unity/unity
      DOWNLOAD_NO_PROGRESS true
      TIMEOUT 30)

    FetchContent_GetProperties(unity_fetch)

    if(NOT unity_fetch_POPULATED)
      FetchContent_Populate(unity_fetch)
    endif()
  endif()

  set(CFLAGS -DUNITY_INCLUDE_CONFIG_H -DUNITY_INCLUDE_DOUBLE)

  target_sources(apps PRIVATE ${UNITY_DIR}/src/unity.c)
  target_include_directories(apps PRIVATE ${UNITY_DIR}/src)
  target_compile_options(apps PRIVATE ${CFLAGS})

endif()
