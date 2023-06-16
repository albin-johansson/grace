cmake_minimum_required(VERSION 3.24)

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
 cmake_path(SET VCPKG_ROOT NORMALIZE $ENV{VCPKG_ROOT})
 set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif ()

project(grace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)

set(GRACE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(GRACE_SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")

set(GRACE_HEADER_FILES
    "${GRACE_INCLUDE_DIR}/grace/context.hpp"
    )

set(GRACE_SOURCE_FILES
    "${GRACE_SOURCES_DIR}/context.cpp"
    )

add_library(grace ${GRACE_HEADER_FILES} ${GRACE_SOURCE_FILES})

target_include_directories(grace
                           PUBLIC
                           ${GRACE_INCLUDE_DIR}

                           SYSTEM PUBLIC
                           "$ENV{VULKAN_SDK}/include"
                           )

target_link_libraries(grace
                      PUBLIC
                      Vulkan::Vulkan
                      unofficial::vulkan-memory-allocator::vulkan-memory-allocator
                      )
