add_executable(advent-of-code-2023 day13/main.cpp)
target_link_libraries(advent-of-code-2023 PRIVATE trak)
set_target_properties(advent-of-code-2023 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})