cmake_minimum_required(VERSION 3.24)

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  cmake_path(SET VCPKG_ROOT NORMALIZE $ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif ()

project(grace LANGUAGES CXX)

# Configuration options
option(GRACE_USE_VULKAN_SUBSET "Allow partial Vulkan implementations, e.g., MoltenVK" OFF)
option(GRACE_USE_SDL2 "Enable SDL2 dependent APIs" OFF)
option(GRACE_ENABLE_DEMO "Build the demo application" OFF)

# Directory variables
set(VULKAN_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
set(GRACE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(GRACE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

# Required dependencies
find_package(Vulkan REQUIRED)
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)

if (GRACE_USE_SDL2 MATCHES ON)
  find_package(SDL2 CONFIG REQUIRED)
endif ()

add_subdirectory(src)

if (GRACE_ENABLE_DEMO MATCHES ON)
  add_subdirectory(demo)
endif ()