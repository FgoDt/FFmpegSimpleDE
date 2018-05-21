# CMakeList.txt : 顶级 CMake 项目文件，执行全局配置
# 和此处的包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("test")

# 包含子项目。
add_subdirectory ("saved")

add_executable(test test.c)

target_link_libraries(
    test
    saved
    
)
