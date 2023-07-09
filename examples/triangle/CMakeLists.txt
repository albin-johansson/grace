project(grace-examples-triangle LANGUAGES CXX)

grace_example(grace-example-triangle ${PROJECT_SOURCE_DIR})

file(COPY "${PROJECT_SOURCE_DIR}/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
