project(grace-src LANGUAGES CXX)

file(GLOB_RECURSE GRACE_SOURCE_FILES
     CONFIGURE_DEPENDS
     ${GRACE_INCLUDE_DIR}/grace/*.hpp
     ${PROJECT_SOURCE_DIR}/*.cpp
     )

add_library(grace ${GRACE_SOURCE_FILES})

set_target_properties(grace
                      PROPERTIES
                      CXX_STANDARD 20
                      CXX_EXTENSIONS OFF
                      )

target_include_directories(grace
                           PUBLIC
                           ${GRACE_INCLUDE_DIR}

                           SYSTEM PUBLIC
                           ${VULKAN_INCLUDE_DIR}
                           )

target_link_libraries(grace
                      PUBLIC
                      Vulkan::Vulkan
                      unofficial::vulkan-memory-allocator::vulkan-memory-allocator
                      )

if (GRACE_USE_VULKAN_SUBSET MATCHES ON)
  target_compile_definitions(grace PUBLIC GRACE_USE_VULKAN_SUBSET)
endif ()

if (GRACE_USE_SDL2 MATCHES ON)
  target_compile_definitions(grace PUBLIC GRACE_USE_SDL2)
  target_link_libraries(grace
                        PUBLIC
                        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
                        )
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang|AppleClang|GNU")
  target_compile_options(grace
                         PRIVATE
                         -Wall
                         -Wextra
                         -Wpedantic
                         -Werror
                         )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(grace
                         PRIVATE
                         /EHsc
                         /MP
                         /W4
                         /bigobj
                         /permissive-
                         /Zc:preprocessor
                         /Zc:__cplusplus
                         )
endif ()

