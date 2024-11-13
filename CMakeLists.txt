cmake_minimum_required(VERSION 3.16)

# Project metadata
project(GDQT VERSION 0.1 LANGUAGES CXX)

# Enable Qt-specific features
set(CMAKE_AUTOUIC ON)    # Auto-compile .ui files
set(CMAKE_AUTOMOC ON)    # Auto-moc for Qt meta-object compiler
set(CMAKE_AUTORCC ON)    # Auto-compile Qt resources

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt5 or Qt6 Widgets module
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Define source files
set(PROJECT_SOURCES
    main.cpp
    questtrackerwindow.cpp
    questtrackerwindow.h
    questtrackerwindow.ui
    jsonparser.h jsonparser.cpp
    gdd_parser.h gdd_parser.cpp
    settings.h settings.cpp
    utils.h utils.cpp
    types.h
    rc.qrc        # Resource file (Qt-specific resources)
    rc.rc         # Windows-specific resource file for app icon
    version.h
    qst_parser.h qst_parser.cpp
)

# Executable target configuration
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # For Qt6 and newer
    qt_add_executable(GDQT
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    # For Qt5 and older, with Android-specific setup if needed
    if(ANDROID)
        add_library(GDQT SHARED ${PROJECT_SOURCES})
    else()
        add_executable(GDQT ${PROJECT_SOURCES})
    endif()
endif()

# Set Windows application icon
if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/rc.rc")
    target_sources(GDQT PRIVATE ${APP_ICON_RESOURCE})
endif()

# Link the Qt Widgets module to the application
target_link_libraries(GDQT PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# macOS bundle settings
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GDQT)
endif()

# Set additional properties for macOS and Windows executables
set_target_properties(GDQT PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE               # Create a macOS bundle
    WIN32_EXECUTABLE TRUE            # Set as a Windows GUI executable
)

# Install configuration
include(GNUInstallDirs)
install(TARGETS GDQT
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize executable for Qt6 (Qt6-specific function)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GDQT)
endif()
