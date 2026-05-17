cmake_minimum_required(VERSION 3.8)
project(ExampleWithFiscionX LANGUAGES C CXX)

if (MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4) # só em MSVC 32-bit
    add_link_options(/SAFESEH:NO)
endif()

add_executable(ExampleWithFiscionX
    source/main.cpp
    source/core.cpp
    source/stb_image.cpp
    source/stb_image_write.cpp
    source/tiny_gltf.cpp
    source/meshoptimizer_simplifier.cpp
    source/allocator.cpp
    source/indexgenerator.cpp
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
else()
    set(ARCH_DIR "x86")
endif()

message(STATUS "Detected CPU Architecture: ${ARCH_DIR}")

target_link_libraries(FiscionX PRIVATE
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/BulletCollision.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/BulletDynamics.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/BulletInverseDynamics.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/BulletSoftBody.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/Bullet3Common.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/fmod_vc.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/freetype.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/glad.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/glfw3dll.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/LinearMath.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/libvlc.lib"
    "${CMAKE_SOURCE_DIR}/redist/${ARCH_DIR}/libvlccore.lib"
)

set_property(TARGET FiscionX PROPERTY CXX_STANDARD 20)
set_property(TARGET FiscionX PROPERTY CXX_STANDARD_REQUIRED ON)