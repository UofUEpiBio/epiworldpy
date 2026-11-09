#include "tool.hpp"
#include "docstrings/tool.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <utility>

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::tool;

static auto new_tool(std::string name, double susceptibility_reduction,
					 double transmission_reduction, double recovery_enhancer,
					 double death_reduction) -> epiworld::Tool {
	Tool tool(std::move(name));

	if (susceptibility_reduction > 0)
		tool.set_susceptibility_reduction(susceptibility_reduction);
	if (transmission_reduction > 0)
		tool.set_transmission_reduction(transmission_reduction);
	if (recovery_enhancer > 0)
		tool.set_recovery_enhancer(recovery_enhancer);
	if (death_reduction > 0)
		tool.set_death_reduction(death_reduction);

	return tool;
}

void epiworldpy::export_tool(pybind11::class_<epiworld::Tool> &c) {
	c.def(py::init(&new_tool), doc::init, py::arg("name"), py::kw_only(),
		  py::arg("susceptibility_reduction") = 0.0,
		  py::arg("transmission_reduction") = 0.0,
		  py::arg("recovery_enhancer") = 0.0, py::arg("death_reduction") = 0.0)
		.def("get_id", &Tool::get_id, doc::get_id)
		.def("get_name", &Tool::get_name, doc::get_name)
		.def("set_name", &Tool::set_name, doc::set_name,
			 py::arg("name"))
		.def("get_date", &Tool::get_date, doc::get_date)
		.def("set_date", &Tool::set_date, doc::set_date,
			 py::arg("date"))
		.def("set_susceptibility_reduction",
			 py::overload_cast<epiworld_double>(
				 &Tool::set_susceptibility_reduction),
			 doc::set_susceptibility_reduction,
			 py::arg("susceptibility_reduction"))
		.def("set_transmission_reduction",
			 py::overload_cast<epiworld_double>(
				 &Tool::set_transmission_reduction),
			 doc::set_transmission_reduction,
			 py::arg("transmission_reduction"))
		.def("set_recovery_enhancer",
			 py::overload_cast<epiworld_double>(
				 &Tool::set_recovery_enhancer),
			 doc::set_recovery_enhancer, py::arg("recovery_enhancer"))
		.def(
			"set_death_reduction",
			py::overload_cast<epiworld_double>(&Tool::set_death_reduction),
			doc::set_death_reduction, py::arg("death_reduction"))
		.def("set_susceptibility_reduction",
			 py::overload_cast<std::string>(
				 &Tool::set_susceptibility_reduction),
			 doc::set_susceptibility_reduction_param,
			 py::arg("param"))
		.def("set_transmission_reduction",
			 py::overload_cast<std::string>(
				 &Tool::set_transmission_reduction),
			 doc::set_transmission_reduction_param,
			 py::arg("param"))
		.def("set_recovery_enhancer",
			 py::overload_cast<std::string>(&Tool::set_recovery_enhancer),
			 doc::set_recovery_enhancer_param, py::arg("param"))
		.def("set_death_reduction",
			 py::overload_cast<std::string>(&Tool::set_death_reduction),
			 doc::set_death_reduction_param, py::arg("param"))
		.def("set_susceptibility_reduction_fun",
			 &Tool::set_susceptibility_reduction_fun,
			 doc::set_susceptibility_reduction_fun, py::arg("fun"))
		.def("set_transmission_reduction_fun",
			 &Tool::set_transmission_reduction_fun,
			 doc::set_transmission_reduction_fun, py::arg("fun"))
		.def("set_recovery_enhancer_fun", &Tool::set_recovery_enhancer_fun,
			 doc::set_recovery_enhancer_fun, py::arg("fun"))
		.def("set_death_reduction_fun", &Tool::set_death_reduction_fun,
			 doc::set_death_reduction_fun, py::arg("fun"))
		.def("add_target", py::overload_cast<int>(&Tool::add_target),
			 doc::add_target, py::arg("target"))
		.def("add_target",
			 py::overload_cast<const Virus &>(&Tool::add_target),
			 doc::add_target, py::arg("target"))
		.def("set_targets", &Tool::set_targets, doc::set_targets,
			 py::arg("lineage_ids"))
		.def("get_targets", &Tool::get_targets, doc::get_targets)
		.def("clear_targets", &Tool::clear_targets, doc::clear_targets)
		.def("targets", &Tool::targets, doc::targets, py::arg("v"))
		.def(
			"clone",
			[](const Tool &self) {
				// A new tool: the model gives it its own id when it is added
				std::unique_ptr<Tool> t = self.clone_ptr();
				t->set_id(-99);
				return t;
			},
			doc::clone)
		.def("print", &Tool::print, doc::print);
}
