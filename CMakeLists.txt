#-------------------------------------------------------------------------------
# \file  CMakeList.txt
# \brief
#-------------------------------------------------------------------------------


cmake_minimum_required (VERSION 2.6)


project(GitPrompt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#--------------------------------------------------------------------------------------------------
# find packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules/CMakeLib")
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(xLib REQUIRED)
find_package(LibXml2 REQUIRED)
#-------------------------------------------------------------------------------
# includes
include_directories("${LIBXML2_INCLUDE_DIR}")
#-------------------------------------------------------------------------------
# sources
set(SOURCES "Main.cpp" "GitPromptApp.cpp")
#-------------------------------------------------------------------------------
# target
add_executable(GitPrompt ${SOURCES})
#-------------------------------------------------------------------------------
pkg_check_modules(PC_XLIB IMPORTED_TARGET xLib)

target_link_libraries(
    ${PROJECT_NAME}
    ${LIBS} ${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS} ${OPENSSL_LIBRARIES}
    # PkgConfig::PC_XLIB
    /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so /usr/lib/x86_64-linux-gnu/libmysqlclient.so /usr/lib/x86_64-linux-gnu/libssh2.so /usr/lib/x86_64-linux-gnu/libcurl.so /usr/lib/x86_64-linux-gnu/libxml2.so /usr/lib/x86_64-linux-gnu/libxcb.so /usr/lib/x86_64-linux-gnu/libX11-xcb.so -lpthread dl pthread rt m z xLib_static
)
#--------------------------------------------------------------------------------------------------
# config
configure_file(
    ${CMAKE_SOURCE_DIR}/Config.h.in
    ${CMAKE_SOURCE_DIR}/Config.h)
#--------------------------------------------------------------------------------------------------
