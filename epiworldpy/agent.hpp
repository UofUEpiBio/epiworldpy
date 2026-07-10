#ifndef EPIWORLDPY_AGENT_HPP
#define EPIWORLDPY_AGENT_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
void export_agent(pybind11::class_<epiworld::Agent<int>> &c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_AGENT_HPP */
