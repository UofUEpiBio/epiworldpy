#include "agent.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;

static auto agent_get_virus(Agent<int> &self) -> py::object {
	auto &v = self.get_virus();
	if (!v)
		return py::none();
	return py::cast(v.get(), py::return_value_policy::reference);
}

static auto agent_get_tools(Agent<int> &self) -> std::vector<Tool<int> *> {
	std::vector<Tool<int> *> out;
	for (auto &t : self.get_tools())
		out.push_back(t.get());
	return out;
}

void epiworldpy::export_agent(py::class_<epiworld::Agent<int>> &c) {
	c.def("get_id", &Agent<int>::get_id, "Get the agent's ID.")
		.def("get_state", &Agent<int>::get_state,
			 "Get the current state index.")
		.def("get_state_prev", &Agent<int>::get_state_prev,
			 "Get the previous state index.")
		.def("get_state_last_changed", &Agent<int>::get_state_last_changed,
			 "Get the day the state last changed.")
		.def("get_virus", &agent_get_virus,
			 "Get the agent's active virus (or None).")
		.def("get_tools", &agent_get_tools,
			 py::return_value_policy::reference_internal,
			 "Get the list of tools this agent carries.")
		.def("get_n_tools", &Agent<int>::get_n_tools,
			 "Get the number of tools this agent carries.")
		.def("get_n_neighbors", &Agent<int>::get_n_neighbors,
			 "Get the number of neighbors.")
		.def("get_n_entities", &Agent<int>::get_n_entities,
			 "Get the number of entities this agent belongs to.")
		.def("has_tool",
			 py::overload_cast<epiworld_fast_uint>(&Agent<int>::has_tool,
												   py::const_),
			 "Check if the agent has a tool by index.", py::arg("t"))
		.def("has_tool",
			 py::overload_cast<std::string_view>(&Agent<int>::has_tool,
												 py::const_),
			 "Check if the agent has a tool by name.", py::arg("name"))
		.def("has_virus",
			 py::overload_cast<epiworld_fast_uint>(&Agent<int>::has_virus,
												   py::const_),
			 "Check if the agent has a virus by index.", py::arg("t"))
		.def("has_virus",
			 py::overload_cast<std::string_view>(&Agent<int>::has_virus,
												 py::const_),
			 "Check if the agent has a virus by name.", py::arg("name"))
		.def("has_entity",
			 py::overload_cast<epiworld_fast_uint>(&Agent<int>::has_entity,
												   py::const_),
			 "Check if the agent belongs to an entity by index.", py::arg("t"))
		.def(
			"change_state",
			[](Agent<int> &self, Model<int> &model,
			   epiworld_fast_uint new_state, epiworld_fast_int queue) {
				self.change_state(model, new_state, queue);
			},
			"Change the agent's state.", py::arg("model"), py::arg("new_state"),
			py::arg("queue") = 0)
		.def(
			"rm_virus",
			[](Agent<int> &self, Model<int> &model) { self.rm_virus(model); },
			"Remove the agent's active virus.", py::arg("model"))
		.def(
			"set_virus",
			[](Agent<int> &self, Model<int> &model, const Virus<int> &virus) {
				self.set_virus(model, virus);
			},
			"Set/infect the agent with a virus.", py::arg("model"),
			py::arg("virus"))
		.def(
			"add_tool",
			[](Agent<int> &self, Model<int> &model, const Tool<int> &tool) {
				self.add_tool(model, tool);
			},
			"Give the agent a tool.", py::arg("model"), py::arg("tool"))
		.def("mutate_virus", &Agent<int>::mutate_virus,
			 "Trigger a virus mutation on this agent.");
}
