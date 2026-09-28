#include "diagram.hpp"
#include "docstrings/diagram.hpp"
#include "config.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::diagram;

void epiworldpy::export_diagram_type(
	pybind11::enum_<epiworld::DiagramType> &e) {
	e.value("Mermaid", epiworld::DiagramType::Mermaid)
		.value("DOT", epiworld::DiagramType::DOT);
}

void epiworldpy::export_diagram(
	pybind11::class_<epiworld::ModelDiagram,
					 std::shared_ptr<epiworld::ModelDiagram>> &c) {
	c.def(py::init<>(), doc::init)
		.def("draw_from_data", &epiworld::ModelDiagram::draw_from_data,
			 doc::draw_from_data, py::arg("diagram_type"), py::arg("states"),
			 py::arg("tprob"), py::arg("fn_output") = "",
			 py::arg("self_loops") = false)
		.def("draw_from_file", &epiworld::ModelDiagram::draw_from_file,
			 doc::draw_from_file, py::arg("diagram_type"),
			 py::arg("fn_transition"), py::arg("fn_output") = "",
			 py::arg("self_loops") = false)
		.def("draw_from_files", &epiworld::ModelDiagram::draw_from_files,
			 doc::draw_from_files, py::arg("diagram_type"),
			 py::arg("fns_transition"), py::arg("fn_output") = "",
			 py::arg("self_loops") = false);
}
