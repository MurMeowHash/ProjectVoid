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
        ${VENDOR_DIR}/stb/lib/stb_image_write.cpp
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
        src/Game/Types/GameObject/GameObject.cpp
        src/Game/Types/GameObject/GameObject.h
        src/Game/ComponentScripts/Transform/Transform.h
        src/Game/Types/Axis.h
        src/Game/Types/Axis.cpp
        src/Core/Resources/ResourceManager.cpp
        src/Core/Resources/ResourceManager.h
        src/Game/Types/Model/Mesh.cpp
        src/Game/Types/Model/Mesh.h
        src/Game/Types/Model/Vertex.h
        src/Game/Types/Model/Model.h
        src/Game/Types/Model/MeshNode.h
        src/Game/Types/GLBuffer/Texture/Texture2D.cpp
        src/Game/Types/GLBuffer/Texture/Texture2D.h
        src/Game/Types/Material.h
        src/Game/Scene/Scene.cpp
        src/Game/Scene/Scene.h
        src/Renderer/RenderData.h
        src/Renderer/Renderer.cpp
        src/Renderer/Renderer.h
        src/Game/ComponentScripts/Camera/Camera.cpp
        src/Game/ComponentScripts/Camera/Camera.h
        src/Game/Types/CreateParameters.h
        src/Game/ComponentScripts/Transform/Transform.cpp
        src/Game/ComponentScripts/ObjectComponent.h
        src/Game/ComponentScripts/MeshRenderData/MeshRenderData.h
        src/Game/ComponentScripts/MeshRenderData/MeshRenderData.cpp
        src/Renderer/Buffers/UBO.cpp
        src/Renderer/Buffers/UBO.h
        src/Dispose/IDisposable.h
        src/Game/Types/Model/Model.cpp
        src/Game/ComponentScripts/ObjectComponent.cpp
        src/Game/ComponentScripts/Movement/Movement.cpp
        src/Game/ComponentScripts/Movement/Movement.h
        src/Game/ComponentScripts/Movement/MouseLook.cpp
        src/Game/ComponentScripts/Movement/MouseLook.h
        src/Renderer/RenderTypes.h
        src/Game/Types/GLBuffer/FrameBuffer/FrameBuffer.cpp
        src/Game/Types/GLBuffer/FrameBuffer/FrameBuffer.h
        src/Game/Types/GLBuffer/RenderBuffer/RenderBuffer.cpp
        src/Game/Types/GLBuffer/RenderBuffer/RenderBuffer.h
        src/Game/Types/GLBuffer/GLBuffer.cpp
        src/Game/Types/GLBuffer/GLBuffer.h
        src/Game/Types/Material.cpp
        src/Game/PostProcessing/PostProcessing.cpp
        src/Game/PostProcessing/PostProcessing.h
        src/Renderer/Buffers/BufferObject.cpp
        src/Renderer/Buffers/BufferObject.h
        src/Renderer/Buffers/VBO.cpp
        src/Renderer/Buffers/VBO.h
        src/Game/ComponentScripts/Light/Light.cpp
        src/Game/ComponentScripts/Light/Light.h
        src/Engine/Physics/Physics.cpp
        src/Engine/Physics/Physics.h
        src/Game/ComponentScripts/Collider/Collider.cpp
        src/Game/ComponentScripts/Collider/Collider.h
        src/Game/Types/BoundingBox/AABB.h
        src/Game/Types/BoundingBox/AABB.cpp
        src/Game/ComponentScripts/Collider/BoxCollider.cpp
        src/Game/ComponentScripts/Collider/BoxCollider.h
        src/Game/ComponentScripts/Rigidbody/Rigidbody.cpp
        src/Game/ComponentScripts/Rigidbody/Rigidbody.h
        src/Engine/Physics/PhysicsConstructionInfo.h
        src/Game/ComponentScripts/Collider/CapsuleCollider.cpp
        src/Game/ComponentScripts/Collider/CapsuleCollider.h
        src/Engine/Physics/Ray.h
        src/Game/ComponentScripts/Test/RayCastTest.cpp
        src/Game/ComponentScripts/Test/RayCastTest.h
        src/Game/ObjectGroup/ObjectGroupManager.cpp
        src/Game/ObjectGroup/ObjectGroupManager.h
)

target_link_libraries(ProjectVoid
        glfw3
        assimp
        BulletDynamics
        BulletCollision
        Bullet3Common
        LinearMath
)