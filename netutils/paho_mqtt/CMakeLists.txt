# ##############################################################################
# apps/netutils/paho_mqtt/CMakeLists.txt
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

if(CONFIG_LIB_MQTT5)

  configure_file(paho_mqtt/src/VersionInfo.h.in
                 ${CMAKE_CURRENT_LIST_DIR}/paho_mqtt/src/VersionInfo.h @ONLY)

  set(MQTT5_INCDIR ${CMAKE_CURRENT_LIST_DIR}/paho_mqtt/src)

  file(GLOB CSRCS paho_mqtt/src/*.c)

  if(CONFIG_OPENSSL_MBEDTLS_WRAPPER)
    list(REMOVE_ITEM CSRCS ${CMAKE_CURRENT_LIST_DIR}/paho_mqtt/src/SHA1.c)
  endif()

  nuttx_add_library(mqtt5)

  target_sources(mqtt5 PRIVATE ${CSRCS})

  target_include_directories(mqtt5 PRIVATE ${MQTT5_INCDIR})

  target_compile_options(mqtt5 PRIVATE ${MQTT5_FLAGS})

  if(CONFIG_UTILS_MQTT5)

    set(MQTT_PUB_FLAGS
        -DmessageArrived=mqtt_pub_messageArrived
        -DonDisconnect=mqtt_pub_onDisconnect
        -DonConnectFailure5=mqtt_pub_onConnectFailure5
        -DonConnectFailure=mqtt_pub_onConnectFailure
        -DonConnect5=mqtt_pub_onConnect5
        -DonConnect=mqtt_pub_onConnect
        -Dmysleep=mqtt_pub_mysleep
        -DtoStop=mqtt_pub_toStop
        -Dopts=mqtt_pub_opts
        -Dmyconnect=mqtt_pub_myconnect
        -Dcfinish=mqtt_pub_cfinish
        -Dtrace_callback=mqtt_pub_trace_callback)

    nuttx_add_application(
      NAME
      mqtt_pub
      SRCS
      paho_mqtt/src/samples/paho_c_pub.c
      paho_mqtt/src/samples/pubsub_opts.c
      DEPENDS
      mqtt5
      INCLUDE_DIRECTORIES
      ${MQTT5_INCDIR}
      COMPILE_FLAGS
      ${MQTT_PUB_FLAGS}
      STACKSIZE
      ${CONFIG_UTILS_MQTT5_STACKSIZE}
      PRIORITY
      ${CONFIG_UTILS_MQTT5_PRIORITY})

    nuttx_add_application(
      NAME
      mqtt_sub
      SRCS
      paho_mqtt/src/samples/paho_c_sub.c
      paho_mqtt/src/samples/pubsub_opts.c
      DEPENDS
      mqtt5
      INCLUDE_DIRECTORIES
      ${MQTT5_INCDIR}
      STACKSIZE
      ${CONFIG_UTILS_MQTT5_STACKSIZE}
      PRIORITY
      ${CONFIG_UTILS_MQTT5_PRIORITY})
  endif()
endif()
