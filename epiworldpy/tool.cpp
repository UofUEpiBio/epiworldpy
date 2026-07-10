#include "tool.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <utility>

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;

static auto new_tool(std::string name, double prevalence, bool as_proportion,
					 double susceptibility_reduction,
					 double transmission_reduction, double recovery_enhancer,
					 double death_reduction) -> epiworld::Tool<int> {
	Tool<int> tool(std::move(name), prevalence, as_proportion);

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

static auto get_tool_state(Tool<int> &self) -> py::dict {
	epiworld_fast_int init, post;
	self.get_state(&init, &post);
	return py::dict(py::arg("init") = init, py::arg("post") = post);
}

static auto get_tool_queue(Tool<int> &self) -> py::dict {
	epiworld_fast_int init, post;
	self.get_queue(&init, &post);
	return py::dict(py::arg("init") = init, py::arg("post") = post);
}

void epiworldpy::export_tool(pybind11::class_<epiworld::Tool<int>> &c) {
	c.def(py::init(&new_tool), "Create a new tool.", py::arg("name"),
		  py::arg("prevalence"), py::arg("as_proportion"),
		  py::arg("susceptibility_reduction") = 0.0,
		  py::arg("transmission_reduction") = 0.0,
		  py::arg("recovery_enhancer") = 0.0, py::arg("death_reduction") = 0.0)
		.def("get_id", &Tool<int>::get_id, "Get the tool's ID.")
		.def("get_name", &Tool<int>::get_name, "Get the tool name.")
		.def("set_name", &Tool<int>::set_name, "Set the tool name.",
			 py::arg("name"))
		.def("get_date", &Tool<int>::get_date, "Get the date introduced.")
		.def("set_date", &Tool<int>::set_date, "Set the date introduced.",
			 py::arg("date"))
		.def("set_susceptibility_reduction",
			 py::overload_cast<epiworld_double>(
				 &Tool<int>::set_susceptibility_reduction),
			 "Set susceptibility reduction (constant).",
			 py::arg("susceptibility_reduction"))
		.def("set_transmission_reduction",
			 py::overload_cast<epiworld_double>(
				 &Tool<int>::set_transmission_reduction),
			 "Set transmission reduction (constant).",
			 py::arg("transmission_reduction"))
		.def("set_recovery_enhancer",
			 py::overload_cast<epiworld_double>(
				 &Tool<int>::set_recovery_enhancer),
			 "Set recovery enhancer (constant).", py::arg("recovery_enhancer"))
		.def(
			"set_death_reduction",
			py::overload_cast<epiworld_double>(&Tool<int>::set_death_reduction),
			"Set death reduction (constant).", py::arg("death_reduction"))
		.def("set_susceptibility_reduction",
			 py::overload_cast<std::string>(
				 &Tool<int>::set_susceptibility_reduction),
			 "Bind susceptibility reduction to a model parameter.",
			 py::arg("param"))
		.def("set_transmission_reduction",
			 py::overload_cast<std::string>(
				 &Tool<int>::set_transmission_reduction),
			 "Bind transmission reduction to a model parameter.",
			 py::arg("param"))
		.def("set_recovery_enhancer",
			 py::overload_cast<std::string>(&Tool<int>::set_recovery_enhancer),
			 "Bind recovery enhancer to a model parameter.", py::arg("param"))
		.def("set_death_reduction",
			 py::overload_cast<std::string>(&Tool<int>::set_death_reduction),
			 "Bind death reduction to a model parameter.", py::arg("param"))
		.def("set_susceptibility_reduction_fun",
			 &Tool<int>::set_susceptibility_reduction_fun,
			 "Set the susceptibility reduction callback.", py::arg("fun"))
		.def("set_transmission_reduction_fun",
			 &Tool<int>::set_transmission_reduction_fun,
			 "Set the transmission reduction callback.", py::arg("fun"))
		.def("set_recovery_enhancer_fun", &Tool<int>::set_recovery_enhancer_fun,
			 "Set the recovery enhancer callback.", py::arg("fun"))
		.def("set_death_reduction_fun", &Tool<int>::set_death_reduction_fun,
			 "Set the death reduction callback.", py::arg("fun"))
		.def("set_state", &Tool<int>::set_state,
			 "Set the state transitions for this tool.", py::arg("init"),
			 py::arg("post"))
		.def("set_queue", &Tool<int>::set_queue,
			 "Set the queue transitions for this tool.", py::arg("init"),
			 py::arg("post"))
		.def("get_state", &get_tool_state, "Get the state transitions.")
		.def("get_queue", &get_tool_queue, "Get the queue transitions.")
		.def("set_distribution", &Tool<int>::set_distribution,
			 "Set the initial distribution function.", py::arg("fun"))
		.def("distribute", &Tool<int>::distribute,
			 "Distribute this tool to agents.", py::arg("model"))
		.def("print", &Tool<int>::print, "Print information about this tool.");
}
