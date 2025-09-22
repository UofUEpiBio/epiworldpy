#ifndef EPIWORLDPY_TOOL_HPP
#define EPIWORLDPY_TOOL_HPP

#include "epiworld.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
void export_tool(pybind11::class_<epiworld::Tool<int>> &c);
}

#endif /* EPIWORLDPY_TOOL_HPP */
