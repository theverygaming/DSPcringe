#I have no idea how to use cmake but this seems to work ¯\_(ツ)_/¯
cmake_minimum_required(VERSION 3.13)

# set the project name and version
project(DSPcringe)

FIND_PACKAGE(PkgConfig REQUIRED)
find_package(sndfile REQUIRED)

add_executable(decimate decimate.c)

target_link_libraries(decimate PUBLIC sndfile)