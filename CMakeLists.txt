#-------------------------------------------------------------------------------
# \file  CMakeList.txt
# \brief
#-------------------------------------------------------------------------------


cmake_minimum_required (VERSION 2.6)


project(GitPrompt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-------------------------------------------------------------------------------
# prepare
if (CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

#--------------------------------------------------------------------------------------------------
# find packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules/CMakeLib")
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(xLib REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Git REQUIRED)
#-------------------------------------------------------------------------------
# includes
include_directories("${LIBXML2_INCLUDE_DIR}")

#-------------------------------------------------------------------------------
# sources
set(SOURCES "Main.cpp" "GitClient.cpp")

#-------------------------------------------------------------------------------
# target
add_executable(GitPrompt ${SOURCES})

#-------------------------------------------------------------------------------
# flags
if     (MSVC)
    set(FLAGS "/W3")
elseif (MINGW)
    set(LIBS ws2_32 psapi uuid ole32 mpr)
    set(FLAGS "-Wall -pipe")
elseif (CMAKE_COMPILER_IS_CLANGXX)
    set(LIBS pthread rt m z)
    set(FLAGS "-pipe -Wall -Wno-deprecated -Wextra -Wconversion -Wshadow -Wno-unused-parameter "
        "-Wno-unused-variable -Wno-unknown-pragmas -Wno-return-type-c-linkage -Werror "
        "-fcolor-diagnostics -Qunused-arguments -U__STRICT_ANSI__ -D__STDC_FORMAT_MACROS")
elseif (CMAKE_COMPILER_IS_GNUCXX)
    set(LIBS pthread rt m z)
    set(FLAGS "-pipe -Wall -Wno-deprecated -Wextra -Wconversion -Wshadow -Wno-unused-parameter "
        "-Wno-unused-variable -Wno-unknown-pragmas -Wno-return-type-c-linkage -Werror "
        "-U__STRICT_ANSI__ -D__STDC_FORMAT_MACROS")
else()
    message("xLib: unknown compiler")
endif()

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        # COMPILE_DEFINITIONS
        COMPILE_FLAGS ${FLAGS}
)

target_link_libraries(
    ${PROJECT_NAME}
    ${LIBS} ${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS} ${OPENSSL_LIBRARIES} /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so /usr/lib/x86_64-linux-gnu/libmysqlclient.so /usr/lib/x86_64-linux-gnu/libssh2.so /usr/lib/x86_64-linux-gnu/libcurl.so /usr/lib/x86_64-linux-gnu/libxml2.so /usr/lib/x86_64-linux-gnu/libxcb.so /usr/lib/x86_64-linux-gnu/libX11-xcb.so -lpthread dl pthread rt m z xLib_static
)
#--------------------------------------------------------------------------------------------------
# config
configure_file(
    ${CMAKE_SOURCE_DIR}/Config.h.in
    ${CMAKE_SOURCE_DIR}/Config.h)
#--------------------------------------------------------------------------------------------------
