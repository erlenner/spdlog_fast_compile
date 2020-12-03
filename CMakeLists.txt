# CMakeLists.txt
#cmake_minimum_required (VERSION 3.16.1)
cmake_minimum_required (VERSION 3.5.1)
project (spdlog_fast_compile)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

add_library(log SHARED log.cpp)
target_link_libraries(log)
install(TARGETS log DESTINATION lib)

add_executable(cpplog main.cpp)
target_link_libraries(cpplog log)
install(TARGETS cpplog DESTINATION bin)

add_executable(clog main.c)
target_link_libraries(clog log)
install(TARGETS clog DESTINATION bin)
