cmake_minimum_required(VERSION 3.22)

project(FileFlow)

set(CMAKE_CXX_STANDARD 20)

set(CORE_SOURCE
        src/core/movingFiles.cpp
        src/core/movingFiles.hpp)

set(QT_SOURCE
        src/qt/mainwindow.hpp
        src/qt/mainwindow.cpp
        src/qt/mainwindow.hpp)

SET(SOURCE src/main.cpp ${CORE_SOURCE} ${QT_SOURCE})

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_add_resources(RESOURCES resources.qrc)

# macOS icon
set(APP_ICON_MACOSX ${CMAKE_SOURCE_DIR}/icons/app_icons.icns)

if(APPLE)
    set_source_files_properties(${APP_ICON_MACOSX}
            PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

qt_add_executable(FileFlow
        WIN32
        ${SOURCE}
        ${RESOURCES}
)

# Windows icon
if(WIN32)
    target_sources(FileFlow PRIVATE app.rc)
endif()

# macOS icon
if(APPLE)
    target_sources(FileFlow PRIVATE ${APP_ICON_MACOSX})
    set_target_properties(FileFlow PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_ICON_FILE app_icons.icns
    )
endif()
target_compile_options(FileFlow PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -Wreturn-type>
)
target_link_libraries(FileFlow PRIVATE Qt6::Core Qt6::Widgets)

find_program(VALGRIND_EXECUTABLE valgrind)
add_custom_target(
        valgrind
        COMMAND ${VALGRIND_EXECUTABLE}
        --tool=memcheck
        --leak-check=full
        --show-leak-kinds=all
        --track-origins=yes
        --log-file=valgrind.log
        $<TARGET_FILE:FileFlow>
        DEPENDS FileFlow
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        VERBATIM
)

find_program(CPPCHECK_EXECUTABLE cppcheck)

add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE}
        --enable=all
        --std=c++20
        --inconclusive
        --quiet
        --inline-suppr
        --suppress=missingIncludeSystem
        --suppress=unusedFunction
        --output-file=${CMAKE_BINARY_DIR}/cppcheck.log
        ${CMAKE_SOURCE_DIR}/src
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
)