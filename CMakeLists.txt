cmake_minimum_required(VERSION 3.29)
project(ProjectVoid)

set(CMAKE_CXX_STANDARD 20)

set(VENDOR_DIR vendor)

file(GLOB_RECURSE VENDOR_INCLUDE LIST_DIRECTORIES true ${VENDOR_DIR}/*/include)
file(GLOB_RECURSE VENDOR_LIBS LIST_DIRECTORIES true ${VENDOR_DIR}/*/lib)

include_directories(${VENDOR_INCLUDE})
link_directories(${VENDOR_LIBS})

set(VENDOR_SRC
        ${VENDOR_DIR}/glad/lib/glad.c
        ${VENDOR_DIR}/stb/lib/stb_image.cpp
)

add_executable(ProjectVoid
        ${VENDOR_SRC}
        src/main.cpp
        src/Shader/Shader.cpp
        src/Shader/Shader.h
        src/Engine/Engine.cpp
        src/Engine/Engine.h
        src/Core/Core.cpp
        src/Core/Core.h
        src/Engine/ExecutionMode.h
        src/Utils/Utils.cpp
        src/Utils/Utils.h
        src/Debug/Debug.cpp
        src/Debug/Debug.h
        src/Error/Error.cpp
        src/Error/Error.h
)

target_link_libraries(ProjectVoid glfw3 assimp)