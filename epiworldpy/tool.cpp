#include "tool.hpp"

#include <utility>

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;

static auto new_tool(std::string name, double prevalence, bool as_proportion,
					 double susceptibility_reduction,
					 double transmission_reduction, double recovery_enhancer,
					 double death_reduction) -> epiworld::Tool<int> {
	Tool<int> tool(std::move(name), prevalence, as_proportion);

	if (susceptibility_reduction > 0) {
		tool.set_susceptibility_reduction(susceptibility_reduction);
	}

	if (transmission_reduction > 0) {
		tool.set_transmission_reduction(transmission_reduction);
	}

	if (recovery_enhancer > 0) {
		tool.set_recovery_enhancer(recovery_enhancer);
	}

	if (death_reduction > 0) {
		tool.set_death_reduction(death_reduction);
	}

	return tool;
}

void epiworldpy::export_tool(pybind11::class_<epiworld::Tool<int>> &c) {
	c.def(py::init(&new_tool), "Create a new tool", py::arg("name"),
		  py::arg("prevalence"), py::arg("as_proportion"),
		  py::arg("susceptibility_reduction") = 0,
		  py::arg("transmission_reduction") = 0,
		  py::arg("recovery_enhancer") = 0, py::arg("death_reduction") = 0)
		.def("get_name", &Tool<int>::get_name, "Get the tool name.")
		.def("set_name", &Tool<int>::set_name, "Set the tool name.",
			 py::arg("name"))
		.def("set_susceptibility_reduction",
			 py::overload_cast<float>(&Tool<int>::set_susceptibility_reduction),
			 "Sets the reduction in agent susceptibility associated with this "
			 "tool.",
			 py::arg("susceptibility_reduction"))
		.def("set_transmission_reduction",
			 py::overload_cast<float>(&Tool<int>::set_transmission_reduction),
			 "Sets the reduction in agent transmission associated with this "
			 "tool.",
			 py::arg("transmission_reduction"))
		.def("set_recovery_enhancer",
			 py::overload_cast<float>(&Tool<int>::set_recovery_enhancer),
			 "Sets the boost in agent recovery associated with this "
			 "tool.",
			 py::arg("recovery_enhancer"))
		.def("set_death_reduction",
			 py::overload_cast<float>(&Tool<int>::set_death_reduction),
			 "Sets the reduction in agent mortality associated with this "
			 "tool.",
			 py::arg("death_reduction"))
		.def("print", &Tool<int>::print, "Print information about this tool.");
}
