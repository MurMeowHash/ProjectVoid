cmake_minimum_required(VERSION 3.29.6)
project(ProjectVoid)

set(CMAKE_CXX_STANDARD 20)

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

file(GLOB_RECURSE VENDOR_INCLUDE LIST_DIRECTORIES true ${VENDOR_DIR}/*/include)
file(GLOB_RECURSE VENDOR_LIBS LIST_DIRECTORIES true ${VENDOR_DIR}/*/lib)

include_directories(${VENDOR_INCLUDE})
link_directories(${VENDOR_LIBS})

set(IMGUI_DIR ${VENDOR_DIR}/imgui/include/imgui)

set(VENDOR_SRC
        ${VENDOR_DIR}/glad/lib/glad.c
        ${VENDOR_DIR}/stb/lib/stb_image.cpp
        ${VENDOR_DIR}/stb/lib/stb_image_write.cpp
)

add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(imgui PUBLIC glfw3 opengl32)

file(GLOB_RECURSE PROJECT_SOURCES
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/*.h"
)

add_executable(ProjectVoid
        ${VENDOR_SRC}
        ${PROJECT_SOURCES}
)

add_custom_command(TARGET ProjectVoid POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/vendor/assimp/lib/libassimp-5.dll"
        $<TARGET_FILE_DIR:ProjectVoid>
        COMMENT "Copying libassimp-5.dll to executable directory"
)

target_include_directories(ProjectVoid PRIVATE
        ${VENDOR_DIR}/nlohmann
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_link_libraries(ProjectVoid
        glfw3
        assimp
        BulletDynamics
        BulletCollision
        Bullet3Common
        LinearMath
        imgui
)
