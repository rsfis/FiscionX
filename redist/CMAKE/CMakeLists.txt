cmake_minimum_required(VERSION 3.8)
project(ExampleWithFiscionX LANGUAGES C CXX)

add_executable(ExampleWithFiscionX
    source/main.cpp
    source/core.cpp
    source/stb_image.cpp
    source/stb_image_write.cpp
    source/tiny_gltf.cpp
)

target_link_libraries(FiscionX PRIVATE
    "${CMAKE_SOURCE_DIR}/redist/x86/BulletCollision.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/BulletDynamics.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/BulletInverseDynamics.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/BulletSoftBody.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/Bullet3Common.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/fmod_vc.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/freetype.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/glad.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/glfw3dll.lib"
    "${CMAKE_SOURCE_DIR}/redist/x86/LinearMath.lib"
)

set_property(TARGET FiscionX PROPERTY CXX_STANDARD 20)
set_property(TARGET FiscionX PROPERTY CXX_STANDARD_REQUIRED ON)