project(malos_service C CXX)
cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11)

find_package(Threads)

add_executable(micarray_dump micarray_dump.cpp)
set_property(TARGET micarray_dump PROPERTY CXX_STANDARD 11)
target_link_libraries(micarray_dump matrix_creator_hal)
target_link_libraries(micarray_dump ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(micarray_dump wiringPi)
