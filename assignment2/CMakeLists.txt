cmake_minimum_required(VERSION 3.10)

project(Rasterizer)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 17)
aux_source_directory(src SRC)

include_directories(/usr/local/include)

add_executable(Rasterizer ${SRC})
target_link_libraries(Rasterizer ${OpenCV_LIBRARIES})
