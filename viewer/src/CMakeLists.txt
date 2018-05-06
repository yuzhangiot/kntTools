cmake_minimum_required(VERSION 2.6.0)

set (CMAKE_CXX_STANDARD 14)

project(kntViewer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

set(GLAD_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/glad/build/include)
set(GLAD_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/glad/build/libglad.a)


find_package(OpenCV REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package (Threads)

pkg_search_module(GLFW REQUIRED glfw3)

file(GLOB srcs *.cpp)

add_definitions(-Dlinux=1)

#set(CMAKE_CXX_FLAGS ${ADDITIONAL_CMAKE_CXX_FLAGS} "-O3 -msse2 -msse3 -Wall -std=c++11")

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -ccbin g++ -O3 -std=c++11)


include_directories(${OPENCV_INCLUDE_DIRS})
#include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLAD_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})

cuda_add_executable(kntViewer
			  ${srcs}
)


target_link_libraries(kntViewer 
					  ${OpenCV_LIBS}
					  Eigen3::Eigen
					  ${GLAD_LIBRARIES}
					  ${GLFW_LIBRARIES}
					  ${CMAKE_THREAD_LIBS_INIT}
					  stdc++fs
)