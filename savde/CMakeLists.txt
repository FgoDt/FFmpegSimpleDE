# CMakeList.txt: savde 的 CMake 项目，包括源和定义
# 此处特定于项目的逻辑。
#
cmake_minimum_required (VERSION 3.8)

include_directories("E:\\BSP\\BSPWindows\\ffmpeg-3.4-win64-dev\\ffmpeg-3.4-win64-dev\\include")
link_directories("E:\\BSP\\BSPWindows\\ffmpeg-3.4-win64-dev\\ffmpeg-3.4-win64-dev\\lib")

# 将源添加到此项目的可执行文件。
add_library (savde "savde.c")

# TODO: 如有需要，请添加测试并安装目标。
