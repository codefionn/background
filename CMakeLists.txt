cmake_minimum_required(VERSION 3.1)
project(background)

# background - background generator
add_executable(background "${background_SOURCE_DIR}/src/main.c"
  "${background_SOURCE_DIR}/src/background.c")
target_include_directories(background PUBLIC "${background_SOURCE_DIR}/include")
target_link_libraries(background png m)
