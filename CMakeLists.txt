project(getfeature_model)
cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)

add_definitions(-DLIBPYIN_BUILD_SHARED)

include_directories(./libpitch/)

aux_source_directory(./libpitch/vamp SRC_LIST)
aux_source_directory(./libpitch/vamp-sdk SRC_LIST)
aux_source_directory(./libpitch/pyin SRC_LIST)

set(CMAKE_BUILD_TYPE Debug)

# 新加的
add_subdirectory(libmfcc)
aux_source_directory(. DIR_SRCS)
add_subdirectory(libwav)

#

add_library(LibPyin SHARED ${SRC_LIST} ./libpitch/normal_dist_pdf.cpp ./libpitch/libpyincpp.cpp ./libpitch/libpyinc.cpp)

add_executable(test_sound main.cpp)
target_link_libraries(test_sound LibPyin)

# 新加的
target_link_libraries(test_sound libmfcc -lm)
target_link_libraries(test_sound libwav -lm)

#