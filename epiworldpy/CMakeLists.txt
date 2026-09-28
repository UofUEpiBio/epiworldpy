# Include the include/epiworld library
include_directories(include/epiworld)

python_add_library(_core MODULE
    agent.cpp
    database.cpp
    diagram.cpp
    entity.cpp
    main.cpp
    model.cpp
    tool.cpp
    virus.cpp
    WITH_SOABI)
target_link_libraries(_core PRIVATE pybind11::headers)
target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

# The vendored epiworld headers use std::iota without including <numeric>.
# GCC and Clang pull it in transitively; MSVC does not.
if(MSVC)
  target_compile_options(_core PRIVATE /FInumeric)
endif()

install(TARGETS _core DESTINATION epiworldpy)
