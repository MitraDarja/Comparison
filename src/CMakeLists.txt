cmake_minimum_required (VERSION 3.8)

# An object library (without main) to be used in multiple targets.
add_library ("${PROJECT_NAME}_lib" STATIC compare.cpp)
target_link_libraries ("${PROJECT_NAME}_lib" PUBLIC seqan3::seqan3)
target_link_libraries ("${PROJECT_NAME}_lib" PUBLIC robin_hood)
target_include_directories ("${PROJECT_NAME}_lib" PUBLIC ../include)
target_link_libraries ("${PROJECT_NAME}_lib" PUBLIC strobemer_lib)

add_executable ("${PROJECT_NAME}" main.cpp)
target_link_libraries ("${PROJECT_NAME}" PRIVATE "${PROJECT_NAME}_lib")
