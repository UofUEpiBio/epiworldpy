#include "agent.hpp"
#include "docstrings/agent.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::agent;

static auto agent_get_virus(Agent &self) -> py::object {
	auto &v = self.get_virus();
	if (!v)
		return py::none();
	return py::cast(v.get(), py::return_value_policy::reference);
}

static auto agent_get_tools(Agent &self) -> std::vector<Tool *> {
	std::vector<Tool *> out;
	for (auto &t : self.get_tools())
		out.push_back(t.get());
	return out;
}

void epiworldpy::export_agent(py::class_<epiworld::Agent> &c) {
	c.def("get_id", &Agent::get_id, doc::get_id)
		.def("get_virus", &agent_get_virus,
			 doc::get_virus)
		.def("get_tools", &agent_get_tools,
			 py::return_value_policy::reference_internal,
			 doc::get_tools)
		.def("get_n_tools", &Agent::get_n_tools,
			 doc::get_n_tools)
		.def("has_tool",
			 py::overload_cast<epiworld_fast_uint>(&Agent::has_tool,
												   py::const_),
			 doc::has_tool_id, py::arg("t"))
		.def("has_tool",
			 py::overload_cast<std::string_view>(&Agent::has_tool,
												 py::const_),
			 doc::has_tool_name, py::arg("name"))
		.def("has_virus",
			 py::overload_cast<epiworld_fast_uint>(&Agent::has_virus,
												   py::const_),
			 doc::has_virus_id, py::arg("t"))
		.def("has_virus",
			 py::overload_cast<std::string_view>(&Agent::has_virus,
												 py::const_),
			 doc::has_virus_name, py::arg("name"))
		.def(
			"set_virus",
			[](Agent &self, Model &model, const Virus &virus) {
				self.set_virus(model, virus);
			},
			doc::set_virus, py::arg("model"),
			py::arg("virus"))
		.def(
			"add_tool",
			[](Agent &self, Model &model, const Tool &tool) {
				self.add_tool(model, tool);
			},
			doc::add_tool, py::arg("model"), py::arg("tool"))
		.def("mutate_virus", &Agent::mutate_virus, doc::mutate_virus,
			 py::arg("model"))
		.def(
			"rm_tool",
			[](Agent &self, Model &model,
			   epiworld_fast_uint tool_idx) { self.rm_tool(model, tool_idx); },
			doc::rm_tool,
			py::arg("model"), py::arg("tool_idx"));
}
