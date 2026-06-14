#====================================================================
# @file        CMakeLists.txt
# @brief       多线程文件搜索项目构建配置文件
# @details     基于 CMake + Qt6 实现跨平台编译、依赖链接、程序打包部署
# @author      我没有会员
# @date        2026-06-14
# @version     V1.0
#====================================================================

# 设定最低兼容 CMake 版本
cmake_minimum_required(VERSION 3.19)

# 定义项目名称与编程语言(C++)
project(FileSearchTool LANGUAGES CXX)

# 查找并加载 Qt6 依赖模块
# Core：Qt核心库  Widgets：桌面控件库  Concurrent：并发组件
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Concurrent)

# Qt 标准工程初始化配置
qt_standard_project_setup()

# 定义可执行程序、所有源码、UI、资源文件
qt_add_executable(FileSearchTool
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    searchengine.h searchengine.cpp
    filterdialog.h filterdialog.cpp
    res.qrc
)

# 链接 Qt 依赖库
target_link_libraries(FileSearchTool
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Concurrent
)

# 安装规则：区分不同平台程序、库文件部署路径
install(TARGETS FileSearchTool
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 生成 Qt 跨平台依赖部署脚本，用于程序发布打包
qt_generate_deploy_app_script(
    TARGET FileSearchTool
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

# 安装部署脚本
install(SCRIPT ${deploy_script})