if(CONFIG_INTERPRETERS_QUICKJS)
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  include(ExternalProject)

  # add quickjs to global INCLUDE DIR
  list(APPEND CFLAGS ${INCDIR_PREFIX}$(APPDIR)/interpreters/quickjs)
  list(APPEND CXXFLAGS ${INCDIR_PREFIX}$(APPDIR)/interpreters/quickjs)
  set_property(
    TARGET nuttx
    APPEND
    PROPERTY NUTTX_INCLUDE_DIRECTORIES ${NUTTX_APPS_DIR}/interpreters/quickjs)
  set(QUICKJS_DIR ${CMAKE_CURRENT_LIST_DIR}/quickjs)
  set(QUICKJS_VERSION "\"2020-11-08\"")
  set(QUICKJS_FLAGS)
  set(QUICKJS_INCDIR)
  set(QUICKJS_CSRCS)
  set(QUICKJS_SRCS)
  set(QUICKAPP_DIR ${NUTTX_APPS_DIR}/frameworks/quickapp)

  if(CONFIG_INTERPRETERS_QUICKJS_MINI)
    list(APPEND QUICKJS_SRCS qjsmini.c)
  endif()

  list(
    APPEND
    QUICKJS_FLAGS
    -Wno-shadow
    -Wno-array-bounds
    -Wno-incompatible-pointer-types
    -Wno-implicit-function-declaration
    -Wno-unused-function
    -Wno-format
    -D__linux__
    -DCONFIG_VERSION=${QUICKJS_VERSION}
    -include
    alloca.h)

  list(APPEND QUICKJS_INCDIR ${QUICKJS_DIR})

  list(
    APPEND
    QUICKJS_CSRCS
    ${QUICKJS_DIR}/quickjs.c
    ${QUICKJS_DIR}/libregexp.c
    ${QUICKJS_DIR}/libbf.c
    ${QUICKJS_DIR}/libunicode.c
    ${QUICKJS_DIR}/cutils.c)

  if(CONFIG_ARCH_ARM)
    list(APPEND QUICKJS_FLAGS -DFE_TONEAREST=0x00000000)
    list(APPEND QUICKJS_FLAGS -DFE_UPWARD=0x00400000)
    list(APPEND QUICKJS_FLAGS -DFE_DOWNWARD=0x00800000)
    list(APPEND QUICKJS_FLAGS -DFE_TOWARDZERO=0x00c00000)
  endif()

  if(CONFIG_INTERPRETERS_QUICKJS_BIGNUM)
    list(APPEND QUICKJS_FLAGS -DCONFIG_BIGNUM)
    list(APPEND QUICKJS_CSRCS ${CMAKE_CURRENT_BINARY_DIR}/qjscalc.c)
  endif()

  if(CONFIG_INTERPRETERS_QUICKJS_DEBUG)
    list(APPEND QUICKJS_CSRCS ${QUICKJS_DIR}/quickjs-debugger.c
         ${QUICKJS_DIR}/quickjs-memorydump.c)
  endif()

  if(CONFIG_INTERPRETERS_QUICKJS_CONFIG_TOOL)
    list(APPEND QUICKJS_FLAGS -DQUICKJS_CONFIG_TOOL)
  endif()

  if(CONFIG_INTERPRETERS_QUICKJS_EXPORT_API)
    list(APPEND QUICKJS_FLAGS -DDISABLE_EXPORT_API)
  endif()

  if(CONFIG_INTERPRETERS_QUICKJS_FULL)
    add_custom_command(
      PRE_BUILD
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/qjsc -c -o
              ${CMAKE_CURRENT_BINARY_DIR}/repl.c -m ${QUICKJS_DIR}/repl.js
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/repl.c
      DEPENDS qjsc)
    add_custom_command(
      PRE_BUILD
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/qjsc -fbignum -c -o
              ${CMAKE_CURRENT_BINARY_DIR}/qjscalc.c ${QUICKJS_DIR}/qjscalc.js
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qjscalc.c
      DEPENDS qjsc)
    list(APPEND QUICKJS_SRCS ${QUICKJS_DIR}/qjs.c
         ${CMAKE_CURRENT_BINARY_DIR}/repl.c)
    list(APPEND QUICKJS_CSRCS ${QUICKJS_DIR}/quickjs-libc.c)
  endif()

  nuttx_add_library(libqjs)
  target_sources(libqjs PRIVATE ${QUICKJS_CSRCS})
  target_include_directories(libqjs PRIVATE ${QUICKJS_INCDIR})
  target_compile_options(libqjs PRIVATE ${QUICKJS_FLAGS})
  nuttx_export_header(TARGET libqjs INCLUDE_DIRECTORIES ${QUICKJS_DIR})

  if(NOT CONFIG_INTERPRETERS_QUICKJS_NONE)
    set(QJS_PRIORITY ${CONFIG_INTERPRETERS_QUICKJS_PRIORITY})
    set(QJS_STACKSIZE ${CONFIG_INTERPRETERS_QUICKJS_STACKSIZE})
    nuttx_add_application(
      NAME
      qjs
      PRIORITY
      ${QJS_PRIORITY}
      STACKSIZE
      ${QJS_STACKSIZE}
      SRCS
      ${QUICKJS_SRCS}
      INCLUDE_DIRECTORIES
      ${QUICKJS_INCDIR}
      COMPILE_FLAGS
      ${QUICKJS_FLAGS}
      DEPENDS
      libqjs)
  endif()

  if(CONFIG_QUICKAPP_AIOTJSC)
    message("build aiotjsc compiler....")
    nuttx_add_application(
      NAME
      aiotjsc
      INCLUDE_DIRECTORIES
      ${QUICKAPP_COMMON_INCLUDES}
      ${QUICKAPP_DIR}/src/jse/quickjs
      ${QUICKAPP_DIR}/src/jse
      ${QUICKAPP_DIR}/src
      SRCS
      ${QUICKAPP_DIR}/src/ajs_compile.cpp
      ${QUICKAPP_DIR}/src/ajs_log.cpp
      ${QUICKAPP_DIR}/src/jse/quickjs/jse_quickjs.cpp
      DEFINITIONS
      -DDEBUG
      -DCONFIG_VERSION=${QUICKJS_VERSION}
      -DAIOTJS_USE_QUICKJS
      -DCONFIG_BIGNUM=${CONFIG_INTERPRETERS_QUICKJS_BIGNUM}
      STACKSIZE
      ${CONFIG_QUICKAPP_STACKSIZE}
      PRIORITY
      ${CONFIG_QUICKAPP_PRIORITY}
      DEPENDS
      libqjs)
  endif(CONFIG_QUICKAPP_AIOTJSC)

  if(CONFIG_QUICKAPP_AIOTJSC OR CONFIG_INTERPRETERS_QUICKJS_FULL)
    set(AIOTJSC_PATH
        ${CMAKE_CURRENT_BINARY_DIR}
        CACHE INTERNAL "")

    ExternalProject_Add(
      qjsc
      SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/quickjs
      BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/
      CMAKE_ARGS -DCONFIG_BIGNUM=${CONFIG_INTERPRETERS_QUICKJS_BIGNUM}
                 -DQUICKJS_VERSION=${QUICKJS_VERSION} -DAPPDIR=${NUTTX_APPS_DIR}
      TEST_COMMAND ""
      INSTALL_COMMAND "")
  endif()

endif()
