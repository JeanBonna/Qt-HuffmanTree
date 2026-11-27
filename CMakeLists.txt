cmake_minimum_required(VERSION 3.19)
project(Qt-HuffmanTree LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Qt-HuffmanTree
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    huffman.cpp
    descompacter.cpp
    compacter.cpp
    file_status.cpp
    node.hpp
    huffman.hpp
    file_status.hpp
    descompacter.hpp
    compacter.hpp
    aboutdialog.h aboutdialog.cpp aboutdialog.ui
)

target_link_libraries(Qt-HuffmanTree
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS Qt-HuffmanTree
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Qt-HuffmanTree
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
