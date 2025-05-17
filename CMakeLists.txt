cmake_minimum_required(VERSION 3.28)
project(Lab_3)

set(CMAKE_CXX_STANDARD 14)

find_package(MPI REQUIRED)

include_directories(SYSTEM ${MPI_CXX_INCLUDE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")

add_executable(Lab_3
        Third_1.cpp)

target_link_libraries(Lab_3 ${MPI_CXX_LIBRARIES})
