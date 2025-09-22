#ifndef EPIWORLDPY_VIRUS_HPP
#define EPIWORLDPY_VIRUS_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
void export_virus_to_agent_fun(
	pybind11::class_<epiworld::VirusToAgentFun<int>> &c);
void export_virus(pybind11::class_<epiworld::Virus<int>> &c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_VIRUS_HPP */
