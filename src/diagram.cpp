#include "diagram.hpp"
#include "config.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;

void epiworldpy::export_diagram_type(
	pybind11::enum_<epiworld::DiagramType> &e) {
	e.value("Mermaid", epiworld::DiagramType::Mermaid)
		.value("DOT", epiworld::DiagramType::DOT);
}

void epiworldpy::export_diagram(
	pybind11::class_<epiworld::ModelDiagram,
					 std::shared_ptr<epiworld::ModelDiagram>> &c) {
	c.def("draw_from_data", &epiworld::ModelDiagram::draw_from_data,
		  "Draw a new diagram from the given data.")
		.def("draw_from_file", &epiworld::ModelDiagram::draw_from_file,
			 "Draw a new diagram from the given file.")
		.def("draw_from_files", &epiworld::ModelDiagram::draw_from_files,
			 "Draw a new diagram from the given files.");
}
