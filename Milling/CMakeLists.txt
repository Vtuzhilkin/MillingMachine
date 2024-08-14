cmake_minimum_required(VERSION 3.16)

project(Milling VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)

qt_add_executable(appMilling
    main.cpp
    millingmachine.h
    millingmachine.cpp
    segment.h
    segment.cpp
    CRC16.h
    CRC16.cpp
)

qt_add_qml_module(appMilling
    URI Milling
    VERSION 1.0
    QML_FILES Main.qml
    RESOURCES resources/MachineFrame.mesh
    RESOURCES resources/YGuides.mesh
    RESOURCES resources/Spindel.mesh
    RESOURCES resources/Table.mesh
    QML_FILES ButtonTemplate.qml
)

set_target_properties(appMilling PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appMilling
    PRIVATE Qt6::Quick
)

install(TARGETS appMilling
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

find_package(Qt6 COMPONENTS SerialPort REQUIRED)
target_link_libraries(appMilling PRIVATE Qt6::SerialPort)

