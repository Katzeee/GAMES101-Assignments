cmake_minimum_required(VERSION 3.10)
set(CMAKE_BUILD_TYPE "Debug")
project(Rasterizer)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 17)
aux_source_directory(src SRC)

include_directories(/usr/local/include ./include)


add_executable(Rasterizer ${SRC})
target_link_libraries(Rasterizer ${OpenCV_LIBRARIES})
#target_compile_options(Rasterizer PUBLIC -Wall -Wextra -pedantic)
