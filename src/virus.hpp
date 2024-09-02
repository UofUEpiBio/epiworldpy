#ifndef EPIWORLDPY_VIRUS_HPP
#define EPIWORLDPY_VIRUS_HPP

#include "interface.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
void export_virus_to_agent_fun(
	pybind11::class_<epiworld::VirusToAgentFun<int>,
					 std::shared_ptr<epiworld::VirusToAgentFun<int>>> &c);
void export_virus(pybind11::class_<epiworld::Virus<int>,
								   std::shared_ptr<epiworld::Virus<int>>> &c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_VIRUS_HPP */