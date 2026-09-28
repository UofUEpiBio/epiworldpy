#include "agent.hpp"
#include "docstrings/agent.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::agent;

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
	c.def("get_id", &Agent<int>::get_id, doc::get_id)
		.def("get_state", &Agent<int>::get_state,
			 doc::get_state)
		.def("get_state_prev", &Agent<int>::get_state_prev,
			 doc::get_state_prev)
		.def("get_state_last_changed", &Agent<int>::get_state_last_changed,
			 doc::get_state_last_changed)
		.def("get_virus", &agent_get_virus,
			 doc::get_virus)
		.def("get_tools", &agent_get_tools,
			 py::return_value_policy::reference_internal,
			 doc::get_tools)
		.def("get_n_tools", &Agent<int>::get_n_tools,
			 doc::get_n_tools)
		.def("get_n_neighbors", &Agent<int>::get_n_neighbors,
			 doc::get_n_neighbors)
		.def("get_n_entities", &Agent<int>::get_n_entities,
			 doc::get_n_entities)
		.def("has_tool",
			 py::overload_cast<epiworld_fast_uint>(&Agent<int>::has_tool,
												   py::const_),
			 doc::has_tool_id, py::arg("t"))
		.def("has_tool",
			 py::overload_cast<std::string_view>(&Agent<int>::has_tool,
												 py::const_),
			 doc::has_tool_name, py::arg("name"))
		.def("has_virus",
			 py::overload_cast<epiworld_fast_uint>(&Agent<int>::has_virus,
												   py::const_),
			 doc::has_virus_id, py::arg("t"))
		.def("has_virus",
			 py::overload_cast<std::string_view>(&Agent<int>::has_virus,
												 py::const_),
			 doc::has_virus_name, py::arg("name"))
		.def("has_entity",
			 py::overload_cast<epiworld_fast_uint>(&Agent<int>::has_entity,
												   py::const_),
			 doc::has_entity, py::arg("t"))
		.def(
			"change_state",
			[](Agent<int> &self, Model<int> &model,
			   epiworld_fast_uint new_state, epiworld_fast_int queue) {
				self.change_state(model, new_state, queue);
			},
			doc::change_state, py::arg("model"), py::arg("new_state"),
			py::arg("queue") = 0)
		.def(
			"rm_virus",
			[](Agent<int> &self, Model<int> &model) { self.rm_virus(model); },
			doc::rm_virus, py::arg("model"))
		.def(
			"set_virus",
			[](Agent<int> &self, Model<int> &model, const Virus<int> &virus) {
				self.set_virus(model, virus);
			},
			doc::set_virus, py::arg("model"),
			py::arg("virus"))
		.def(
			"add_tool",
			[](Agent<int> &self, Model<int> &model, const Tool<int> &tool) {
				self.add_tool(model, tool);
			},
			doc::add_tool, py::arg("model"), py::arg("tool"))
		// Agent::mutate_virus() calls Virus::mutate() without the model it
		// needs and does not compile, so we mutate the virus directly.
		.def(
			"mutate_virus",
			[](Agent<int> &self, Model<int> &model) {
				if (self.get_virus() == nullptr)
					throw std::logic_error("Agent " +
										   std::to_string(self.get_id()) +
										   " has no virus to mutate.");
				self.get_virus()->mutate(&model);
			},
			doc::mutate_virus, py::arg("model"))
		.def("has_neighbor", &Agent<int>::has_neighbor,
			 doc::has_neighbor,
			 py::arg("neighbor_id"))
		.def("get_neighbors", &Agent<int>::get_neighbors,
			 py::return_value_policy::reference_internal,
			 doc::get_neighbors, py::arg("model"))
		.def(
			"rm_tool",
			[](Agent<int> &self, Model<int> &model,
			   epiworld_fast_uint tool_idx) { self.rm_tool(model, tool_idx); },
			doc::rm_tool,
			py::arg("model"), py::arg("tool_idx"))
		.def(
			"add_entity",
			[](Agent<int> &self, Model<int> &model, Entity<int> &entity) {
				self.add_entity(model, entity);
			},
			doc::add_entity, py::arg("model"), py::arg("entity"))
		.def(
			"rm_entity",
			[](Agent<int> &self, Model<int> &model, Entity<int> &entity) {
				self.rm_entity(model, entity);
			},
			doc::rm_entity, py::arg("model"),
			py::arg("entity"))
		.def("get_entities", &Agent<int>::get_entities,
			 doc::get_entities);
}
