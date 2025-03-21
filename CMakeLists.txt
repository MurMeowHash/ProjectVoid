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
        src/Renderer/RenderItem3D.h
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
        src/Engine/Time/Time.cpp
        src/Engine/Time/Time.h
        src/Engine/Input/Input.cpp
        src/Engine/Input/Input.h
        src/Engine/Input/InputKey.h
        src/Engine/Input/InputMouseButton.h
        src/Game/Types/GameObject/GameObject.cpp
        src/Game/Types/GameObject/GameObject.h
        src/Game/Types/Transform/Transform.h
        src/Game/Types/Axis.h
        src/Game/Types/Axis.cpp
        src/Core/Resources/ResourceManager.cpp
        src/Core/Resources/ResourceManager.h
        src/Game/Types/Model/Mesh.cpp
        src/Game/Types/Model/Mesh.h
        src/Game/Types/Model/Vertex.h
        src/Game/Types/Model/Model.h
        src/Game/Types/Model/MeshNode.h
        src/Game/Types/Texture/Texture2D.cpp
        src/Game/Types/Texture/Texture2D.h
        src/Game/Types/Material.h
        src/Game/Scene/Scene.cpp
        src/Game/Scene/Scene.h
        src/Renderer/RenderData.h
        src/Renderer/Renderer.cpp
        src/Renderer/Renderer.h
        src/Game/Types/Camera/Camera.cpp
        src/Game/Types/Camera/Camera.h
        src/Game/Types/CreateParameters.h
        src/Game/Types/Transform/Transform.cpp
        src/Game/ComponentScripts/ObjectComponent.h
        src/Game/Types/Model/MeshRenderData.h
        src/Game/Types/Model/MeshRenderData.cpp
        src/Renderer/Buffers/UBO.cpp
        src/Renderer/Buffers/UBO.h
        src/Dispose/IDisposable.h
        src/Game/Types/Model/Model.cpp
        src/Game/ComponentScripts/ObjectComponent.cpp
        src/Game/ComponentScripts/Movement/Movement.cpp
        src/Game/ComponentScripts/Movement/Movement.h
        src/Game/ComponentScripts/Movement/MouseLook.cpp
        src/Game/ComponentScripts/Movement/MouseLook.h
)

target_link_libraries(ProjectVoid glfw3 assimp)