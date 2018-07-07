cmake_minimum_required(VERSION 3.11.1)

project(KeyenceLkG5000)

# The static library for KEYENCE LK-G5000 series controller
add_subdirectory(libLkG5000)

# Application project to test the library. Excluded from any parent project of KeyenceLkG5000.
add_subdirectory(test)
