# CMakeList.txt: saved 的 CMake 项目，包括源和定义
# 此处特定于项目的逻辑。
#
cmake_minimum_required (VERSION 3.8)

IF(UNIX)
    include_directories("../Thirdparty/FFmpeg/linux/include")
    link_directories("../Thirdparty/FFmpeg/linux/lib")
ELSEIF(WIN32)
    include_directories("E:\\BSP\\BSPWindows\\ffmpeg-3.4-win64-dev\\ffmpeg-3.4-win64-dev\\include")
    link_directories("E:\\BSP\\BSPWindows\\ffmpeg-3.4-win64-dev\\ffmpeg-3.4-win64-dev\\lib")
ENDIF()



# 将源添加到此项目的可执行文件。
add_library (saved

                STATIC

                saved.c
                log.c
                internal.c
                codec.c
                decoder.c
                encoder.c
                format.c
                audioresample.c
                videoscale.c
                videofilter.c)


