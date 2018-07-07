cmake_minimum_required(VERSION 3.11.1)

project(TestLkG5000)

set(CMAKE_INCLUDE_CURRENT_DIR ON)   # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON)               # Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON)               # Create code from a list of Qt designer ui files

# Packages included in project
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

# Get the application files for execution and to display in Visual Studio Project directories.
file(GLOB PROJECT_FILES "*.h"
                        "*.cpp"
                        "*.ui")

# Add dependent-library directory to the include paths.
include_directories(${libLkG5000_INCLUDE_DIRS})

# Output the executable
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_FILES})

# Link agaist libraries
target_link_libraries(${PROJECT_NAME} 
                        Qt5::Core 
                        Qt5::Gui
                        Qt5::Widgets
                        libLkG5000)
