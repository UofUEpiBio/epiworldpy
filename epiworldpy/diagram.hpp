#ifndef EPIWORLDPY_DIAGRAM_HPP
#define EPIWORLDPY_DIAGRAM_HPP

#include "epiworld.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
void export_diagram_type(pybind11::enum_<epiworld::DiagramType> &e);

void export_diagram(
	pybind11::class_<epiworld::ModelDiagram,
					 std::shared_ptr<epiworld::ModelDiagram>> &c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_DIAGRAM_HPP */
