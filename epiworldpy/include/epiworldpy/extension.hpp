#ifndef EPIWORLDPY_EXTENSION_HPP
#define EPIWORLDPY_EXTENSION_HPP

/**
 * @file extension.hpp
 * @brief Extending epiworldpy in C++: reach a model made in Python and use
 * the whole epiworld C++ API on it.
 *
 * @details Include this header (and nothing else of epiworld) in C++ that
 * works on epiworldpy's models, compiled in a session with
 * `epiworldpy.cpp_source()` or in a package with the include directory
 * `epiworldpy.get_include()`:
 *
 * ```cpp
 * #include <epiworldpy/extension.hpp>
 *
 * void add_my_event(pybind11::object model) {
 *     epiworld::Model & m = epiworldpy::as_model(model);
 *     m.add_globalevent(...);
 * }
 * ```
 *
 * epiworldpy uses epiworld's default configuration; do not change it.
 * `as_model()` checks that the extension and the installed epiworldpy use the
 * same major version of epiworld.
 */

#include <numeric> // epiworld uses std::iota (MSVC does not include it)
#include <string>
#include <pybind11/pybind11.h>

#include "../epiworld/epiworld.hpp"

namespace epiworldpy {

/**
 * @brief The model behind an epiworldpy model (`Model()`,
 * `epimodels.ModelSIR()`, ...).
 * @details Everything the C++ API can do to a model is then available: add
 * global events, tools, viruses with their natural histories, layers,
 * parameters... What is added belongs to the model, as if added from Python.
 * The reference is valid while the Python model exists. Raises `TypeError` if
 * `model` is not an epiworldpy model, and `RuntimeError` if this code was
 * built against another major version of epiworld than the installed
 * epiworldpy's (rebuild it).
 */
static inline epiworld::Model & as_model(pybind11::handle model)
{

    namespace py = pybind11;

    py::module_ core = py::module_::import("epiworldpy._core");

    const std::string version =
        core.attr("__epiworld_version__").cast< std::string >();
    const int installed = std::stoi(version);
    if (installed != EPIWORLD_VERSION_MAJOR)
        throw std::runtime_error(
            "This C++ code was built for epiworld " +
            std::to_string(EPIWORLD_VERSION_MAJOR) + ", but epiworldpy uses "
            "epiworld " + version + ". Rebuild it against the installed "
            "epiworldpy."
        );

    if (!py::isinstance(model, core.attr("Model")))
        throw py::type_error("`model` is not an epiworldpy model.");

    py::capsule capsule = model.attr("_capsule")();
    return *capsule.get_pointer< epiworld::Model >();

}

}

#endif
