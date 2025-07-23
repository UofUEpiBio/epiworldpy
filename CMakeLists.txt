cmake_minimum_required(VERSION 3.15...3.26)

project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES CXX)

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Include the include/epiworld library
include_directories(include/epiworld)

python_add_library(_core MODULE
    src/database.cpp
    src/diagram.cpp
    src/entity.cpp
    src/main.cpp
    src/model.cpp
    src/saver.cpp
    src/tool.cpp
    src/virus.cpp
    WITH_SOABI)
target_link_libraries(_core PRIVATE pybind11::headers)
target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

install(TARGETS _core DESTINATION epiworldpy)
