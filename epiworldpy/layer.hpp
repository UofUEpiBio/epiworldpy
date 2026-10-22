#ifndef EPIWORLDPY_LAYER_HPP
#define EPIWORLDPY_LAYER_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
/* The `graph` (network generators, fixed graphs, rewiring) and `layer`
 * (contact layers) submodules, mirroring epiworld's graph:: and layer::. */
void export_graph(pybind11::module &m);
void export_layer(pybind11::module &m);
} // namespace epiworldpy

#endif /* EPIWORLDPY_LAYER_HPP */
