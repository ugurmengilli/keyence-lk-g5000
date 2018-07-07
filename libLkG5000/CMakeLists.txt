cmake_minimum_required(VERSION 3.11.1)

project(libLkG5000)

file(GLOB PROJECT_FILES "*.h"
						"*.cpp")
						
set(CMAKE_INCLUDE_CURRENT_DIR ON)   # Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)   # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON)               # Instruct CMake to run moc automatically when needed.

# Packages included in project
find_package(Qt5Core REQUIRED)
find_package(Qt5SerialPort REQUIRED)

# Output a static library
add_library(${PROJECT_NAME} ${PROJECT_FILES})
					  
# Link agaist Qt libraries
target_link_libraries(${PROJECT_NAME} Qt5::Core
									  Qt5::SerialPort)

# Expose the project's public includes to other subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
                                 ${Qt5Core_INCLUDE_DIRS}
                                 ${Qt5SerialPort_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
