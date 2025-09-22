# Include the include/epiworld library
include_directories(include/epiworld)

python_add_library(_core MODULE
    database.cpp
    diagram.cpp
    entity.cpp
    main.cpp
    model.cpp
    saver.cpp
    tool.cpp
    virus.cpp
    WITH_SOABI)
target_link_libraries(_core PRIVATE pybind11::headers)
target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

install(TARGETS _core DESTINATION epiworldpy)
