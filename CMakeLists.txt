#--------------------------------------------------------------------------------------------------
# \file  CMakeList.txt
# \brief
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5.0)

project(GitPrompt)
#--------------------------------------------------------------------------------------------------
# Options
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#--------------------------------------------------------------------------------------------------
# find packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules/CMakeLib")
# message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

find_package(xLib    REQUIRED)
find_package(LibXml2 REQUIRED)
#--------------------------------------------------------------------------------------------------
# target
add_executable(${PROJECT_NAME}
    GitPromptApp.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${LIBXML2_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
    ${cmXLIB_LIBRARIES}
    ${cmXLIB_SYS_LIBRARIES}
    ${cmXLIB_EXTRA_LIBRARIES} )
#--------------------------------------------------------------------------------------------------
# config
configure_file(
    ${CMAKE_SOURCE_DIR}/Config.h.in
    ${CMAKE_SOURCE_DIR}/Config.h)
#--------------------------------------------------------------------------------------------------
