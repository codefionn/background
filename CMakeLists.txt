cmake_minimum_required(VERSION 3.1)
project(background)

find_package(PNG REQUIRED)

# background - background generator
add_executable(background "${background_SOURCE_DIR}/src/main.c"
  "${background_SOURCE_DIR}/src/background.c")
target_include_directories(background PUBLIC
  "${background_SOURCE_DIR}/include"
  ${PNG_INCLUDE_DIR})
target_link_libraries(background ${PNG_LIBRARY} m)

install(TARGETS background DESTINATION bin)
