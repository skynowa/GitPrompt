#-------------------------------------------------------------------------------
# \file  CMakeList.txt
# \brief
#-------------------------------------------------------------------------------


cmake_minimum_required (VERSION 2.6)


project(GitPrompt)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#--------------------------------------------------------------------------------------------------
# find packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules/CMakeLib")
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(xLib    REQUIRED)
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
target_link_libraries(${PROJECT_NAME} ${XLIB_LIBRARIES})
#--------------------------------------------------------------------------------------------------
# config
configure_file(
    ${CMAKE_SOURCE_DIR}/Config.h.in
    ${CMAKE_SOURCE_DIR}/Config.h)
#--------------------------------------------------------------------------------------------------
