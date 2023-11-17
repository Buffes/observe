cmake_minimum_required(VERSION 3.16)

project(observe VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick Quick3D)

qt_standard_project_setup(REQUIRES 6.5)

set(CMAKE_AUTORCC ON)

qt_add_executable(appobserve
    #images.qrc
    main.cpp
)

qt_add_qml_module(appobserve
    URI observe
    VERSION 1.0
    SOURCES calculate_positions.h calculate_positions.cpp
    SOURCES planetmodel.h planetmodel.cpp
    SOURCES starInstanceTable.h starInstanceTable.cpp
    SOURCES datastructures.h
    QML_FILES
        Main.qml
        MyCalendar.qml
)


qt_add_resources(appobserve "assets"
    PREFIX "/img"
    FILES
#        "fish.png"
#        "celestial_grid_16k_print.jpg"
#        "celestial_grid_16k.tif"
        "constellation_figures_4k.tif"
)

qt_add_resources(appobserve "shaders"
    PREFIX "/shaders"
    FILES
        "star.vert"
        "star.frag"
)

#qt6_add_lightprobe_images(appobserve "hdrs"
#    PREFIX "/hdr"
#    FILES
#        "milkyway_2020_4k.exr"
#    OUTPUTS
#        "skybox.ktx"
#)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appobserve PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appobserve
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appobserve PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Quick3D
)

include(GNUInstallDirs)
install(TARGETS appobserve
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
