# CMakeLists.txt
#cmake_minimum_required (VERSION 3.16.1)
cmake_minimum_required (VERSION 3.5.1)
project (spdlog_fast_compile)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

file(GLOB SOURCES *.cpp)
add_executable(spdlog_fast_compile ${SOURCES})

#target_precompile_headers(spdlog2 PRIVATE include/spdlog.h)

target_link_libraries(spdlog_fast_compile)

install(TARGETS spdlog_fast_compile DESTINATION bin)
