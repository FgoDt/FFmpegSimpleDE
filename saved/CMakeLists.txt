# CMakeList.txt: saved 的 CMake 项目，包括源和定义
# 此处特定于项目的逻辑。
#
cmake_minimum_required (VERSION 3.8)



# 将源添加到此项目的可执行文件。
add_library (saved  saved.c
                    log.c
                    format.c
                    internal_context.c
)


# TODO: 如有需要，请添加测试并安装目标。
