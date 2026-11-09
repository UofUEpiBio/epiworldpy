#ifndef EPIWORLDPY_ASSIGN_HPP
#define EPIWORLDPY_ASSIGN_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
/* The `assign` submodule's placement object (see natural_history.hpp). */
void export_assign(pybind11::module &m);
} // namespace epiworldpy

#endif /* EPIWORLDPY_ASSIGN_HPP */
