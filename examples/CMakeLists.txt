project(grace-examples LANGUAGES CXX)

set(GRACE_EXAMPLE_BASE_DIR "${PROJECT_SOURCE_DIR}/base")

function(grace_example name src_dir)
  file(GLOB_RECURSE GRACE_EXAMPLE_SOURCE_FILES
       CONFIGURE_DEPENDS
       ${src_dir}/*.cpp
       ${src_dir}/*.hpp
       ${GRACE_EXAMPLE_BASE_DIR}/*.cpp
       ${GRACE_EXAMPLE_BASE_DIR}/*.hpp
       )

  add_executable(${name} ${GRACE_EXAMPLE_SOURCE_FILES})

  set_target_properties(${name}
                        PROPERTIES
                        CXX_STANDARD 20
                        CXX_EXTENSIONS OFF
                        )

  target_link_libraries(${name}
                        PRIVATE
                        grace
                        imgui::imgui
                        glm::glm
                        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                        )

  target_include_directories(${name}
                             PRIVATE
                             ${GRACE_EXAMPLE_BASE_DIR}
                             )

  target_compile_definitions(${name}
                             PRIVATE
                             GLM_FORCE_RADIANS
                             GLM_FORCE_DEPTH_ZERO_TO_ONE
                             )

  grace_enable_compiler_warnings(${name})
endfunction()

add_subdirectory(window)
