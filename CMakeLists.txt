cmake_minimum_required(VERSION 3.16)

project(MillingMachine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick SerialPort)

qt_standard_project_setup(REQUIRES 6.5)
set(RESOURCE_FILES resources.qrc)

set(CMAKE_AUTORCC ON)
set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")

qt_add_executable(appMillingMachine
    main.cpp
    shaders.qrc
    MillingMachine.h
    MillingMachine.cpp
    Message.h
    Message.cpp
    ${app_icon_resource_windows}
)

qt_add_qml_module(appMillingMachine
    URI MillingMachine
    VERSION 1.0
    QML_FILES Main.qml
    QML_FILES GradientRectangle.qml
    QML_FILES GradientButton.qml
    QML_FILES GradientText.qml
    QML_FILES GradientTextEdit.qml
    QML_FILES COMport.qml
    QML_FILES RuleButtons.qml
    QML_FILES PositionButtons.qml
    QML_FILES Status.qml
    QML_FILES Window3D.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appMillingMachine PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appMillingMachine
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appMillingMachine
    PRIVATE Qt6::Quick Qt6::SerialPort
)

include(GNUInstallDirs)
install(TARGETS appMillingMachine
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
