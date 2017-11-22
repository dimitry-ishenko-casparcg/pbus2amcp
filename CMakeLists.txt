cmake_minimum_required(VERSION 3.1)
project(pbus2amcp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Network Gui Widgets REQUIRED)

set(HEADERS
    gui/casparcg.hpp
    gui/pbus.hpp
    gui/reg.hpp
    gui/window.hpp
    src/casparcg.hpp
)

set(SOURCES
    gui/casparcg.cpp
    gui/pbus.cpp
    gui/reg.cpp
    gui/window.cpp
    src/casparcg.cpp
    src/main.cpp
)

set(FORMS
    gui/casparcg.ui
    gui/pbus.ui
    gui/reg.ui
    gui/window.ui
)

set(RESOURCES
    res/icons.qrc
)

add_executable(pbus2amcp ${SOURCES} ${FORMS} ${RESOURCES})
target_link_libraries(pbus2amcp Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets)
