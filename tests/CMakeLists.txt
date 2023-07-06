project(grace-tests LANGUAGES CXX)

file(GLOB_RECURSE GRACE_TEST_SOURCE_FILES
     CONFIGURE_DEPENDS
     ${PROJECT_SOURCE_DIR}/src/*.cpp
     )

add_executable(grace-tests ${GRACE_TEST_SOURCE_FILES})

set_target_properties(grace-tests
                      PROPERTIES
                      CXX_STANDARD 20
                      CXX_EXTENSIONS OFF
                      )

target_include_directories(grace-tests PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(grace-tests
                      PRIVATE
                      grace
                      GTest::gtest
                      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                      )

file(COPY "assets/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
