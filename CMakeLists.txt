cmake_minimum_required(VERSION 3.15)

project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Find Python for configuring and building
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

# Add venv pybind11 for development
execute_process(
  COMMAND ${Python_EXECUTABLE} -c
    "import sysconfig; print(sysconfig.get_path('platlib'))"
  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Python site-packages: ${PYTHON_SITE_PACKAGES}")
list(APPEND CMAKE_PREFIX_PATH "${PYTHON_SITE_PACKAGES}/pybind11/share/cmake/pybind11")

# Use pybind11 from venv or system
find_package(pybind11 CONFIG REQUIRED)

add_subdirectory("epiworldpy")
