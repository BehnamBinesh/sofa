cmake_minimum_required(VERSION 3.22)

project(CImgPlugin_test)

sofa_find_package(CImgPlugin REQUIRED)

set(SOURCE_FILES
    ImageCImg_test.cpp
)

add_definitions("-DCIMGPLUGIN_RESOURCES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/images\"")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} CImgPlugin Sofa.Core Sofa.Testing)


