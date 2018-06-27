# CMakeList.txt : 顶级 CMake 项目文件，执行全局配置
# 和此处的包含子项目。
#
cmake_minimum_required (VERSION 3.8)

include_directories("/home/fftest/FFmpeg/fflibs/linux64/include")
link_directories("/home/fftest/FFmpeg/fflibs/linux64/lib")

add_definitions("-Wall -g")

project ("est")

# 包含子项目。
add_subdirectory ("saved")


add_executable(est test.c)

target_link_libraries(
    est
    saved
    
    avformat
    avcodec
    avfilter
    swscale
    swresample
    avutil

    m 
    pthread
)
