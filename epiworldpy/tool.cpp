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
	c.def(py::init(&new_tool), doc::init, py::arg("name"),
		  py::arg("prevalence"), py::arg("as_proportion"),
		  py::arg("susceptibility_reduction") = 0.0,
		  py::arg("transmission_reduction") = 0.0,
		  py::arg("recovery_enhancer") = 0.0, py::arg("death_reduction") = 0.0)
		.def("get_id", &Tool<int>::get_id, doc::get_id)
		.def("get_name", &Tool<int>::get_name, doc::get_name)
		.def("set_name", &Tool<int>::set_name, doc::set_name,
			 py::arg("name"))
		.def("get_date", &Tool<int>::get_date, doc::get_date)
		.def("set_date", &Tool<int>::set_date, doc::set_date,
			 py::arg("date"))
		.def("set_susceptibility_reduction",
			 py::overload_cast<epiworld_double>(
				 &Tool<int>::set_susceptibility_reduction),
			 doc::set_susceptibility_reduction,
			 py::arg("susceptibility_reduction"))
		.def("set_transmission_reduction",
			 py::overload_cast<epiworld_double>(
				 &Tool<int>::set_transmission_reduction),
			 doc::set_transmission_reduction,
			 py::arg("transmission_reduction"))
		.def("set_recovery_enhancer",
			 py::overload_cast<epiworld_double>(
				 &Tool<int>::set_recovery_enhancer),
			 doc::set_recovery_enhancer, py::arg("recovery_enhancer"))
		.def(
			"set_death_reduction",
			py::overload_cast<epiworld_double>(&Tool<int>::set_death_reduction),
			doc::set_death_reduction, py::arg("death_reduction"))
		.def("set_susceptibility_reduction",
			 py::overload_cast<std::string>(
				 &Tool<int>::set_susceptibility_reduction),
			 doc::set_susceptibility_reduction_param,
			 py::arg("param"))
		.def("set_transmission_reduction",
			 py::overload_cast<std::string>(
				 &Tool<int>::set_transmission_reduction),
			 doc::set_transmission_reduction_param,
			 py::arg("param"))
		.def("set_recovery_enhancer",
			 py::overload_cast<std::string>(&Tool<int>::set_recovery_enhancer),
			 doc::set_recovery_enhancer_param, py::arg("param"))
		.def("set_death_reduction",
			 py::overload_cast<std::string>(&Tool<int>::set_death_reduction),
			 doc::set_death_reduction_param, py::arg("param"))
		.def("set_susceptibility_reduction_fun",
			 &Tool<int>::set_susceptibility_reduction_fun,
			 doc::set_susceptibility_reduction_fun, py::arg("fun"))
		.def("set_transmission_reduction_fun",
			 &Tool<int>::set_transmission_reduction_fun,
			 doc::set_transmission_reduction_fun, py::arg("fun"))
		.def("set_recovery_enhancer_fun", &Tool<int>::set_recovery_enhancer_fun,
			 doc::set_recovery_enhancer_fun, py::arg("fun"))
		.def("set_death_reduction_fun", &Tool<int>::set_death_reduction_fun,
			 doc::set_death_reduction_fun, py::arg("fun"))
		.def("set_state", &Tool<int>::set_state,
			 doc::set_state, py::arg("init"),
			 py::arg("post"))
		.def("set_queue", &Tool<int>::set_queue,
			 doc::set_queue, py::arg("init"),
			 py::arg("post"))
		.def("get_state", &get_tool_state, doc::get_state)
		.def("get_queue", &get_tool_queue, doc::get_queue)
		.def("set_distribution", &Tool<int>::set_distribution,
			 doc::set_distribution, py::arg("fun"))
		.def("distribute", &Tool<int>::distribute,
			 doc::distribute, py::arg("model"))
		.def("print", &Tool<int>::print, doc::print);
}
