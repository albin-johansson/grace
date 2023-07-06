project(grace-demo LANGUAGES CXX)

file(GLOB_RECURSE GRACE_DEMO_SOURCE_FILES
     CONFIGURE_DEPENDS
     ${PROJECT_SOURCE_DIR}/src/*.cpp
     ${PROJECT_SOURCE_DIR}/src/*.hpp
     )

add_executable(grace-demo ${GRACE_DEMO_SOURCE_FILES})

set_target_properties(grace-demo
                      PROPERTIES
                      CXX_STANDARD 20
                      CXX_EXTENSIONS OFF
                      )

target_link_libraries(grace-demo
                      PRIVATE
                      grace
                      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                      )

file(COPY "assets/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
